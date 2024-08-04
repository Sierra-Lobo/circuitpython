#include "linearAlgebra.h"
#include <stdlib.h>
#include "../Igrf/Igrf.h"
#include "math.h"
#define PI 3.14159265358979323846
#define RAD2DEG (180.0/PI)
#define RE 6387100 
#define MU 3.986e14
typedef struct EkfInfo {
	double bi[3];
	double x[7];
	double state[6];
	double P[7][7];
	double Q[7][7];
	double R[6][6];
	double nmax;
	double MOI[3][3];
	double MOI_Inv[3][3];
	double T[3];
	double dt;
	
} EkfInfo;

void construct_ekf(EkfInfo* ekf);

void llaToEcef(double lla[3], double xyz[3]);
void rk4(void (*fun_ptr)(EkfInfo*, double*, size_t, double, double*),EkfInfo*,  double* x , size_t n, double t, double dt);
void step(EkfInfo* info, double T[3], double imu[6], double gps[3], double dt);
