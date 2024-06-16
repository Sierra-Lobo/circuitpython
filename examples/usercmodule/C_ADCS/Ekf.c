#include "Ekf.h"


void construct_ekf(EkfInfo* ekf)
{
	memset(ekf, 0, sizeof(EkfInfo));
	ekf->x[0] = 1;
	mSetIdentity(ekf->P, 7, 7);
	mSetIdentity(ekf->Q, 7, 7);
	mSetIdentity(ekf->R, 6, 6);


	ekf->nmax = extrapsh(2024);
	m33Set(.95, -.30028, .00329, 
			.30023, .95382, .00979,
			-.00607, -.00835, .99995, ekf->MOI);
	for (int i=0; i<3; i++) {
		ekf->MOI[0][i] *= 18126705.0557671;
		ekf->MOI[1][i] *= 18184363.4289426;
		ekf->MOI[2][i] *= 4293944.40168418;
	}
	m33Scale(1e-9, ekf->MOI, ekf->MOI);
	m33Inverse(ekf->MOI, ekf->MOI_Inv);

	for (int i=0; i<4; i++) ekf->Q[3+ i][3+i] = 10;

	mScale(1e-8 * 1e6,ekf->Q, 7, 7, ekf->Q);

	double a = (.5* RAD2DEG);
	a*=a;
	double b = (.3 * RAD2DEG);
    b*=b;	

	ekf->R[0][0] = a;
	ekf->R[1][1] = a;
	for (int i=0; i<4; i++) {
		ekf->R[2 + i][2+i] = b;
	}
	//initialize P
	mCopy(ekf->R, 6, 6, ekf->P);
	mScale(10, ekf->R, 6, 6, ekf->P);
	ekf->P[6][6] = b;
	ekf->P[0][0]*=3;	
	ekf->P[1][1]*=3;	

	ekf->dt = 1;
	ekf->state[0] = 400000+ RE;
	double inc = 62.5;
	double v = sqrt(MU / ekf->state[0]);
	ekf->state[4] = v*sin(inc);
	ekf->state[5] = v*cos(inc);


}
void rk4(void (*fun_ptr)(EkfInfo*, double*, size_t, double, double*),EkfInfo* info,  double* x, size_t n,double t, double dt)
{
	double k1[7] /*= (double*) malloc(n*sizeof(double))*/;
	double k2[7] /*= (double*) malloc(n*sizeof(double))*/;
	double k3[7] /*= (double*) malloc(n*sizeof(double))*/;
	double k4[7] /*= (double*) malloc(n*sizeof(double))*/;

	
	
	fun_ptr(info, x, n, t, k1);

	vScale(.5*dt, k1, n, k2);
	vAdd(k1, n, x, k2);
	fun_ptr(info, k2, n, t + .5 * dt, k2);
	
	vScale(.5*dt, k2, n, k3);
	vAdd(k3, n, x, k2);
	fun_ptr(info, k3, n, t, k3);


	vScale(.5*dt, k3, n, k4);
	vAdd(k4, n, x, k4);
	fun_ptr(info, k4, n, t, k4);

	vScale(2, k2, n, k2);
	vScale(2, k3, n, k3);

	vAdd(k1, n, k2, k2);
	vAdd(k2, n, k3, k3);
	vAdd(k3, n, k4, k4);

	vScale(dt / 6, k4, n, k4);

	vAdd(x, n, k4, x);
/*
	free(k1);
	free(k2);
	free(k3);
	free(k4);
*/
}

void llaToEcef(double lla[3], double xyz[3])
{
	double rad = 6378137.0;
	double f = 1 / 298.257223564;
	double cosLat = cos(lla[0] /RAD2DEG);
	double sinLat = sin(lla[0] /RAD2DEG);

	double FF = (1-f) * (1-f);
	double v[2] = {cosLat, sinLat};
	double C = 1 / v2Norm(v);
	double S = C * FF;

	xyz[0] = (rad * C + lla[2]) * cosLat * cos(lla[1] / RAD2DEG);
	xyz[1] = (rad * C + lla[2]) * cosLat * sin(lla[1] / RAD2DEG);
	xyz[2] = (rad * S + lla[2]) * sinLat;
}

static void rToPtp(double r[3], double ptp[3])
{
	double rho = v3Norm(r);
	ptp[0] = 0;
	ptp[1] = acos(r[2] / rho);
	ptp[2] = atan2(r[1], r[0]);
}
/*
static void ptpToLla(double ptp[3], double rho, double lla[3])
{
	lla[0] = 90 - ptp[1] * RAD2DEG;
	lla[1] = ptp[2] * RAD2DEG; 
	lla[2] = rho - RE;

}
*/

static void updateBModel(EkfInfo* info, double gps[3], double bb[3])
{
    double NED[3], Tib[3][3];
	double Tbody[3][3];
	double ptp[3], ecef[3];
	VEC field;
	shval3(64.9261111/RAD2DEG,-147.4958333/RAD2DEG,6371.2,info->nmax,&field);
	

	v3Set(field.c.x, field.c.y, field.c.z, NED);
	v3Scale(1e-9, NED, NED);

	llaToEcef(gps, ecef);
	rToPtp(ecef, ptp);
	TIB(ptp, Tib);

	mMultV(Tib, 3, 3, NED, info->bi);
	
	TfromQ( Tbody, info->x);

	mMultV(Tbody, 3, 3, info->bi, bb);
	
}



static void state_dynamics(EkfInfo* info, double* x, size_t n, double t, double* res)
{
	double H[3], wxh[3] ;
	double Tcross[3];

	quatDerivative(x, x+4, res);
	mtMultV(info->MOI, 3, 3, x+4, H);
	v3Cross(x+4, H, wxh);
	
	v3Subtract(info->T, wxh, Tcross);
	mMultV(info->MOI_Inv, 3, 3, Tcross, res+4);	

	
}

static void dwdotdw(EkfInfo* info, double m[3][3])
{
	double j1 = info->MOI[0][0];
	double j2 = info->MOI[1][1];
	double j3 = info->MOI[2][2];
	//double* w = info->x + 4;
	double w[3];
	v3Copy(info->x + 4, w);


	double a =  w[2] * (j2-j3)/j1;
	double b = w[1] * (j2-j3)/j1;

	double c = w[2] * (j3-j1)/j2;
	double d =  w[0] * (j3-j1)/j2;

	double e =  w[1] * (j1-j2)/j3;
   	double f =	w[0] * (j1-j2)/j3;

	m33Set( 0,a,b,
			c,0,d,
			e,f,0,
		   	m);

}
/*
#define MXINDEX(dim2, row, col) ((row)*(dim2) + (col))
static void printMat(void* m, size_t dim1, size_t dim2)
{
	double* mat = (double*)m;
	for (size_t i=0; i<dim1; i++) {
		for (size_t j=0; j<dim2; j++) {
			printf("[%d,%d] = %f\n", i, j, mat[MXINDEX(dim2, i, j)]);
			
		}

	}

}
*/
static void dfdx(EkfInfo* info, double F[7][7])
{
	double a11[4][4];
	double a12[4][3];
	double a21[3][4];
	double a22[3][3];
	omegaW(info->x + 4, a11);
	mScale(.5, a11, 4, 4, a11);
	omegaQ(info->x, a12);
	mScale(.5, a12, 4, 3, a12);


	mSetZero(a21, 3, 4);
	dwdotdw(info, a22);



	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++)
			F[i][j] = a11[i][j];
	}
	for (int i=0; i<4; i++) {
		for (int j=0; j<3; j++) {
			F[i][4 + j] = a12[i][j];
			F[j+4][i] = a21[j][i];
		}
	}
	for (int i=0; i<3; i++) {
		for (int j=0; j<3; j++) {
			F[4+i][4+j] = a22[i][j];
		}
	
	}
	
}


static void predict(EkfInfo* info, double T, double dt)
{
	double F[7][7];

	double FP[7][7], FPT[7][7];
	rk4(state_dynamics,info, info->x, 7, T, dt );
	

	dfdx(info, F);

	mMultM(F, 7, 7, info->P, 7, 7, FP);
	
	mTranspose(F, 7, 7, FPT);

	mMultM(info->P, 7, 7, FPT, 7, 7, FPT);

	mAdd(FP, 7, 7, FPT, FPT);
	mAdd(FPT, 7, 7, info->Q, FPT);


	mScale(dt, FPT, 7, 7, FPT);


	mAdd(FPT, 7, 7, info->P, info->P);

}


static void update(EkfInfo* info, double imu[6])
{
	double bhat[3];
	double a1[3][3], a2[3][3], a3[3][3], a4[3][3];
	double c1[3], c2[3], c3[3], c4[3];
	double h[6][7];

	mSetZero(h, 6, 7);
	v3Normalize(info->bi, bhat);
	dAdq1(info->x, a1);	
	dAdq2(info->x, a2);	
	dAdq3(info->x, a3);	
	dAdq4(info->x, a4);

	
	m33MultV3(a1, bhat, c1);
	m33MultV3(a2, bhat, c2);
	m33MultV3(a3, bhat, c3);
	m33MultV3(a4, bhat, c4);

	for (int i=0; i<3; i++) {
		h[i][0] = c1[i];
		h[i][1] = c2[i];
		h[i][2] = c3[i];
		h[i][3] = c4[i];

		h[i + 3][i+4] = 1;
	}

	double pmh[7][6];
	double hmb[6][7];
	double ht[7][6];
	double R1[6][6];
	double R1I[6][6];
	double K[7][6];
	mSetZero(R1I, 6, 6);
	mTranspose(h, 6, 7, ht);
	
	mMultM(info->P, 7, 7, ht, 7, 6, pmh);
	mMultM(h, 6, 7, info->P, 7, 7, hmb);
	mMultM(hmb, 6, 7, ht, 7, 6, R1);
	
	

	mAdd(R1, 6, 6, info->R, R1);
	
	mInverse(R1, 3, R1I);
	mMultM(pmh, 7, 6, R1I, 6, 6, K);

	double newState[4];

	mMultV(K, 4, 3, imu, newState);

	vAdd(newState, 4, info->x, info->x);
	vCopy(imu+3, 3, info->x + 4);


	double I7[7][7];
	double kmh[7][7];
	double pL[7][7];
	mSetIdentity(I7, 7, 7);
	mMultM(K, 7, 6, h, 6, 7, kmh);
	mSubtract(I7, 7, 7, kmh, pL);
	mMultM(pL, 7, 7, info->P, 7, 7, info->P); 

	vNormalize(info->x, 4, info->x);


	






}


void step(EkfInfo* info, double T[3], double imu[6], double gps[3], double dt)
{
	double bb[3] ;

	info->dt = dt;
	updateBModel(info, gps, bb);

	v3Subtract(imu, bb, imu);
	
	predict(info, 0, dt);

	update(info, imu);


}
