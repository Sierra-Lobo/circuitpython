#include "Adcs.h"

void two_body_ode(EkfInfo* eInfo, double state[6], size_t n, double t, double res[6])
{
	double accel[3];
	double rho;

	rho = v3Norm(state);

	v3Scale(-MU / (rho*rho*rho), state, accel);

	memcpy(state+3, res, 3*sizeof(double));
	memcpy(accel, res+3, 3*sizeof(double));	

}


void getGps(EkfInfo* eInfo, double gps[3])
{
	if (gps != NULL)
	{
		 llaToEcef(gps, eInfo->state);	
		
	}
	rk4(two_body_ode,eInfo,  eInfo->state, 6,0, eInfo->dt);
}


void single_iteration(AdcsInfo* adcs, double imu[6], double gps[3], double dipole[3])
{
	if (adcs->mode == BDOT)  bdot(&adcs->cInfo, imu,  dipole);
	
	else {
		printf("adcs getting gps\n");
		getGps(&adcs->eInfo, gps);
		printf("adcs getting ekf step\n");
		step(&adcs->eInfo, adcs->eInfo.T, imu, gps, adcs->dt);
		printf("adcs getting arl checl\n");
		if (vNorm(adcs->eInfo.x + 4, 3) > ARL) {
			adcs->mode = BDOT;
		}
		
		if (adcs->mode == SUN_POINTING) {
			return;
		}
		if (adcs->mode == NADIR_POINTING) {

		printf("adcs getting nadir\n");
		nadir(&adcs->cInfo, adcs->eInfo.x, imu, adcs->eInfo.bi, adcs->eInfo.x, dipole);
		}

	}
		printf("adcs success\n");
}
