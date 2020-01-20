#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "controller_mellinger.h"
#include "motors.h"
#include "power_distribution.h"
#include "stabilizer_types.h"

static setpoint_t setpoint;
static sensorData_t sensorData;
static state_t state;
static control_t control;
static int motors[4];

static float randu(float min, float max)
{
  return ((max - min) / RAND_MAX) * rand() + min;
}

static quaternion_t randquat()
{
  quaternion_t q;
  q.x = randu(-1, 1);
  q.y = randu(-1, 1);
  q.z = randu(-1, 1);
  q.w = randu(-1, 1);
  float const s = 1.0f / sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  q.x *= s;
  q.y *= s;
  q.z *= s;
  q.w *= s;
  return q;
}

static vector_t randvec(float box)
{
  vector_t v = {
    .x = randu(-box, box),
    .y = randu(-box, box),
    .z = randu(-box, box)
  };
  return v;
}

static Axis3f randAxis3f(float box)
{
  Axis3f a = {
    .x = randu(-box, box),
    .y = randu(-box, box),
    .z = randu(-box, box)
  };
  return a;
}

static attitude_t randattitude(float box)
{
  attitude_t a = {
    .roll = randu(-box, box),
    .pitch = randu(-box, box),
    .yaw = randu(-box, box)
  };
  return a;
}

static void randomizeSetpoint()
{
  setpoint.attitude = randattitude(180.0f);
  setpoint.attitudeRate = randattitude(1000.0f);
  setpoint.attitudeQuaternion = randquat();
  setpoint.thrust = randu(0.0f, 1000.0f); // TODO: What should upper limit be?
  setpoint.position = randvec(1);
  setpoint.velocity = randvec(1);
  setpoint.acceleration = randvec(1);
  setpoint.velocity_body = false;

  // Only test trajectory-tracking mode for now.
  setpoint.mode.x = modeAbs;
  setpoint.mode.y = modeAbs;
  setpoint.mode.z = modeAbs;
  setpoint.mode.quat = modeAbs;
}

static void randomizeState()
{
  state.attitude = randattitude(180.0f);
  state.attitudeQuaternion = randquat();
  state.position = randvec(1);
  state.velocity = randvec(1);
  state.acc = randvec(1);
}

static void randomizeSensorData()
{
  sensorData.acc = randAxis3f(4.0f);
  sensorData.gyro = randAxis3f(1000.0f);
  // Sparkfun's digital compass datasheet listed 6 Gauss as its maximum sensitivity.
  sensorData.mag = randAxis3f(6.0f);
}

static void stabilizer()
{
  // controller_mellinger uses .gyro and .acc from sensorData
  controllerMellinger(&control, &setpoint, &sensorData, &state);
  powerDistribution(&control);
  for (int i = MOTOR_M1; i <= MOTOR_M4; ++i) {
    motors[i] = motorsGetRatio(i);
  }
}


int main()
{
  int const N_TRIALS = 1000;
  int const STEPS_PER_TRIAL = 10;
  srand(100);

  for (int i = 0; i < N_TRIALS; ++i) {
    controllerMellingerInit();
    randomizeSetpoint();
    randomizeState();
    randomizeSensorData();
    for (int j = 0; j < STEPS_PER_TRIAL; ++j) {
      stabilizer();
      printf("%d, %d, %d, %d\n", motors[0], motors[1], motors[2], motors[3]);
    }
  }
}
