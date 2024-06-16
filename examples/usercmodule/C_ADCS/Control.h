#include "linearAlgebra.h"
//#define PI 3.141592653589793

typedef struct ControlInfo {
	double kBdot;
	double KP[3][3];
	double KD[3][3];
	
	double wd[3];
	double w0[3];

} ControlInfo;

void construct_control(ControlInfo* control);

void bdot(ControlInfo* info, double y[6], double dipole[3]);
void nadir(ControlInfo* info, double x[7], double bb[3], double bi[3], double state[6], double dipole[3]);
