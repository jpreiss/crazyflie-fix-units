/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * motors.c - Motor driver
 *
 * This code mainly interfacing the PWM peripheral lib of ST.
 */

#include <stdbool.h>

#include "motors.h"
#include "math3d.h"


uint32_t motor_ratios[] = {0, 0, 0, 0};

/* Private functions */

static float pmGetBatteryVoltage()
{
  return 3.7f;
}


/* Public functions */

// Ithrust is thrust mapped for 65536 <==> 60 grams TOTAL thrust (15g per motor)
void motorsSetThrust(uint32_t id, float thrust)
{
  uint16_t ratio;

  float volts = -(SQR(1000.0f) * SQR(4.0)) * 0.0006239f * thrust * thrust + (1000.0f * 4.0) * 0.088f * thrust;
  float supply_voltage = pmGetBatteryVoltage();
  float percentage = volts / supply_voltage;
  percentage = percentage > 1.0f ? 1.0f : percentage;
  ratio = percentage * UINT16_MAX;
  motor_ratios[id] = ratio;
}

void motorsSetRatio(uint32_t id, uint16_t ithrust)
{
  uint16_t ratio;
  float thrust = ((float)ithrust / 65536.0f) * 60;
  float volts = -0.0006239f * thrust * thrust + 0.088f * thrust;
  float supply_voltage = pmGetBatteryVoltage();
  float percentage = volts / supply_voltage;
  percentage = percentage > 1.0f ? 1.0f : percentage;
  ratio = percentage * UINT16_MAX;
  motor_ratios[id] = ratio;
}

int motorsGetRatio(uint32_t id)
{
  return motor_ratios[id];
}

