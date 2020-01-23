/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2011-2016 Bitcraze AB
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
 * power_distribution_stock.c - Crazyflie stock power distribution code
 */
#include "power_distribution.h"

#include <string.h>
#include "num.h"
#include "motors.h"

static struct {
  uint32_t m1;
  uint32_t m2;
  uint32_t m3;
  uint32_t m4;
} motorPower;

#define limitThrust(VAL) limitUint16(VAL)


void powerDistribution(const control_t *control)
{
  float const r_torque = RP_INERTIA * control->angular_accel.x;
  float const p_torque = RP_INERTIA * control->angular_accel.y;
  // float const y_torque = YAW_INERTIA * control->angular_accel.z;

  float const r = r_torque / (4.0f * RP_ARM);
  float const p = p_torque / (4.0f * RP_ARM);
  float const y = control->yaw / 4.0f;

  motorPower.m1 = limitThrust((MASS/4.0) * control->z_accel - r + p + y);
  motorPower.m2 = limitThrust((MASS/4.0) * control->z_accel - r - p - y);
  motorPower.m3 =  limitThrust((MASS/4.0) * control->z_accel + r - p + y);
  motorPower.m4 =  limitThrust((MASS/4.0) * control->z_accel + r + p - y);

  motorsSetRatio(MOTOR_M1, motorPower.m1);
  motorsSetRatio(MOTOR_M2, motorPower.m2);
  motorsSetRatio(MOTOR_M3, motorPower.m3);
  motorsSetRatio(MOTOR_M4, motorPower.m4);
}

