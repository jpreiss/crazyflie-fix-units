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
  float m1;
  float m2;
  float m3;
  float m4;
} motorThrust;

// account for old units
#define limitThrust(VAL) clamp((VAL) * (0.015 / 65536), 0.0, 0.015)


void powerDistribution(const control_t *control)
{
  float const r_torque = RP_INERTIA * control->angular_accel.x;
  float const p_torque = RP_INERTIA * control->angular_accel.y;
  float const y_torque = YAW_INERTIA * control->angular_accel.z;

  float const r = r_torque / (4.0f * RP_ARM);
  float const p = p_torque / (4.0f * RP_ARM);
  float const y = y_torque / (4.0f * TORQUE_THRUST_RATIO);

  motorThrust.m1 = limitThrust((MASS/4.0) * control->z_accel - r + p + y);
  motorThrust.m2 = limitThrust((MASS/4.0) * control->z_accel - r - p - y);
  motorThrust.m3 =  limitThrust((MASS/4.0) * control->z_accel + r - p + y);
  motorThrust.m4 =  limitThrust((MASS/4.0) * control->z_accel + r + p - y);

  motorsSetThrust(MOTOR_M1, motorThrust.m1);
  motorsSetThrust(MOTOR_M2, motorThrust.m2);
  motorsSetThrust(MOTOR_M3, motorThrust.m3);
  motorsSetThrust(MOTOR_M4, motorThrust.m4);
}

