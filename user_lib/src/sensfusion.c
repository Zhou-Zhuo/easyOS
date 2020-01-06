#include <math.h>
#include "sensfusion.h"

float q0 = 1.0f;
float q1 = 0.0f;
float q2 = 0.0f;
float q3 = 0.0f;  // quaternion of sensor frame relative to auxiliary frame

float beta = 0.01f;

float twoKp = 2.0 * 1.0;
float twoKi = 2.0 * 0.001;

float integralFBx = 0.0f;
float integralFBy = 0.0f;
float integralFBz = 0.0f;  // integral error terms scaled by Ki


void sensfusion6UpdateQ(float gx, float gy, float gz, float ax, float ay, float az, float dt)
{
  float recipNorm;
  float halfvx, halfvy, halfvz;
  float halfex, halfey, halfez;
  float qa, qb, qc;

  gx = gx * M_PI / 180;
  gy = gy * M_PI / 180;
  gz = gz * M_PI / 180;

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
  {
    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // Estimated direction of gravity and vector perpendicular to magnetic flux
    halfvx = q1 * q3 - q0 * q2;
    halfvy = q0 * q1 + q2 * q3;
    halfvz = q0 * q0 - 0.5f + q3 * q3;

    // Error is sum of cross product between estimated and measured direction of gravity
    halfex = (ay * halfvz - az * halfvy);
    halfey = (az * halfvx - ax * halfvz);
    halfez = (ax * halfvy - ay * halfvx);

    // Compute and apply integral feedback if enabled
    if(twoKi > 0.0f)
    {
      integralFBx += twoKi * halfex * dt;  // integral error scaled by Ki
      integralFBy += twoKi * halfey * dt;
      integralFBz += twoKi * halfez * dt;
      gx += integralFBx;  // apply integral feedback
      gy += integralFBy;
      gz += integralFBz;
    }
    else
    {
      integralFBx = 0.0f; // prevent integral windup
      integralFBy = 0.0f;
      integralFBz = 0.0f;
    }

    // Apply proportional feedback
    gx += twoKp * halfex;
    gy += twoKp * halfey;
    gz += twoKp * halfez;
  }

  // Integrate rate of change of quaternion
  gx *= (0.5f * dt);   // pre-multiply common factors
  gy *= (0.5f * dt);
  gz *= (0.5f * dt);
  qa = q0;
  qb = q1;
  qc = q2;
  q0 += (-qb * gx - qc * gy - q3 * gz);
  q1 += (qa * gx + qc * gz - q3 * gy);
  q2 += (qa * gy - qb * gz + q3 * gx);
  q3 += (qa * gz + qb * gy - qc * gx);

  // Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
}


void sensfusion6GetEulerRPY(float* roll, float* pitch, float* yaw, uint8_t arg)
{
  //float tanyaw   = 2 * (q1*q2 - q0*q3) / (q0*q0 - q1*q1 + q2*q2 - q3*q3);
  //float tanroll  = 2 * (q1*q3 - q0*q2) / (q0*q0 - q1*q1 - q2*q2 + q3*q3);
  //float sinpitch = 2 * (q2*q3 + q0*q1);

  *roll  = atan2(2 * (q1*q3 - q0*q2), 2 * (q0*q0 - q1*q1 - q2*q2 + q3*q3));
  *yaw   = atan2(2 * (q1*q2 - q0*q3), 2 * (q0*q0 - q1*q1 + q2*q2 - q3*q3));
  *pitch = asin(0 - 2 * (q2*q3 + q0*q1));
}
//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float invSqrt(float x)
{
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;
  i = 0x5f3759df - (i>>1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}

