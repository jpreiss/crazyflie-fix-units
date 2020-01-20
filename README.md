Converting the Crazyflie firmware to use SI units
=================================================

Abstract
--------
We replace arbitrary hardware-dependent units within the Crazyflie 2.0 quadrotor control firmware with appropriate SI units.
Our method uses automated randomized testing and dimensional analysis to preserve carefully-tuned controller gain parameters.


Introduction
------------

Ideally, a robot's control system should work in physical SI units almost everywhere.
Sensor and actuator implementation details such as voltage ranges, PWM duty cycles, integer quantizations, and so on, should only appear in a thin hardware abstraction layer.

The Crazyflie 2.0 firmware lacks this property, probably for the following reasons:

1. The Crazyflie 1.0 CPU had no floating-point unit, so integer fixed-point math was necessary.
2. Without system identification of thrust and inertial parameters, it's not possible to convert from SI units to units like "motor PWM ratio" anyway.

With the Crazyflie 1.0 support dropped from the firmware, and good system identification available [1], we should now try to fix the issue.

The main challenge lies in the goal of preserving hand-tuned controller parameters.
They represent many hours of manual tuning effort and cannot be re-derived from scratch.
Therefore, we must carefully figure out the units of the current tuning parameters so we can convert them to the proper SI unit.

To assist with this goal, we want to write a test harness that captures the entire behavior of the controller.
The state and setpoint inputs to the controller are already specified in known units.
The output of the controller is an array of four 16-bit unsigned integer values representing PWM ratios for the motors.
We will isolate all relevant parts of the control system into a form that can be compiled for x86 and easily tested.
Then, we can "fuzz" the current system with many random inputs and record the PWM outputs into a file.
As we modify the code to introduce proper units, we can verify that the output values do not change significantly.
(Some change is to be expected due to rounding errors.)

We will focus on the Mellinger controller in trajectory-tracking mode, since that is the controller used in our own work.
We hope other members from the Crazyflie community can apply the same methodology to the controllers that are part of their standard workflow.


Development Environment
-----------------------
Many parts of the Crazyflie 2.0 firmware depend directly or transitively on ARM-specific and sensor-specific headers.
For the sake of testing, we need to isolate the controller computation from all this.
We begin by copying the relevant source files to a new directory:

	controller_mellinger.c
	motors.c
	num.c
	power_distribution_stock.c
	stabilizer.c

We then write a generic makefile that compiles all C sources, and start removing irrelevant code that causes compile errors.
Instead of documenting each change we need to make, we will make a series of commits to this repository, so the diffs can be inspected.


[1] Förster, Julian. System Identification of the Crazyflie 2.0 Nano Quadrocopter. Bachelor Thesis. ETH Zürich. 2015.
