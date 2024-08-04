#include "Control.h"
#include "Ekf.h"
#include <stdint.h>
#include <stdlib.h>
#define BDOT 0
#define SUN_POINTING 1
#define NADIR_POINTING 2
#define ARL 10 //deg/s
#define MU 3.986e14
typedef struct AdcsInfo {
	ControlInfo cInfo;
	EkfInfo eInfo;
	uint8_t mode;
	double dt;

} AdcsInfo;

void getGps(EkfInfo* eInfo, double gps[3]);
void two_body_ode(EkfInfo* eInfo, double state[6], size_t n, double t, double res[6]);
void single_iteration(AdcsInfo* adcs, double imu[6], double gps[3], double dipole[3]);
