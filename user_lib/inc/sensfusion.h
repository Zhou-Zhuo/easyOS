#ifndef SENSFUSION_H
#define SENSFUSION_H
#include <stdint.h>

#define M_PI 3.14159265358979323846
void sensfusion6UpdateQ(float gx, float gy, float gz, float ax, float ay, float az, float dt);
void sensfusion6GetEulerRPY(float* roll, float* pitch, float* yaw, uint8_t arg);
float invSqrt(float x);

#endif 
