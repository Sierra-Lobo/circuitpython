#include "Control.h"


void construct_control(ControlInfo* control)
{
	control->kBdot = 672000;
	v3Set(0, -2* 3.141562653587973 / (93*60), 0, control->w0);
	m33Set(293.486, .5515, -9.7049,
		-.0069, 299.8118, -4.112, 
		4.8505, -.1118, 299.8613, control->KP);
	m33SetIdentity(control->KD);
	
	m33Scale(65./50., control->KP, control->KP);
	m33Scale(1.8 * 1e4 * 100, control->KD, control->KD);	

}

void bdot(ControlInfo* info, double y[6], double dipole[3])
{
	double bxw[3];

	v3Cross(y+3, y, bxw);
	
	v3Scale(info->kBdot, bxw, dipole);

	v3Limit(dipole, -.19, .19);

}



void nadir(ControlInfo* info, double x[7], double bb[3], double bi[3], double state[6], double dipole[3])
{
	double CLN[3][3], CNL[3][3];
	double qln[4], qrl[4], qrn[4], qerr[4];
	double werr[3], w0[3], dcross[3], pcross[3], p[3], d[3];

	TOrbit2Inertial(state, CNL);
	m33Transpose(CNL, CLN);

	qfromT(qln, CLN);
	v4Set(.5,.5,.5,.5, qrl);
	

	qErr(qrl, qln, qrn);

	qErr(qrn, x, qerr);


	mMultV(CNL, 3, 3, info->w0, w0);

	v3Subtract(x + 4, w0, werr);
	v3Cross( werr, bb, dcross);

	v3Cross(qerr, bb, pcross);

	m33MultV3(info->KD, dcross, d);
	m33MultV3(info->KP, pcross, p);
	
	
	v3Add(d, p, dipole);

	

	v3Limit(dipole, -.19, .19);
}
