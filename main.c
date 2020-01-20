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
  stabilizer();
}
