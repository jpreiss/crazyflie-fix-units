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


Development Process
-------------------
Many parts of the Crazyflie 2.0 firmware depend directly or transitively on ARM-specific and sensor-specific headers.
For the sake of testing, we need to isolate the controller computation from all this.
In the initial commit, we copy the relevant source files to a new directory and write a generic makefile.
Then we start removing irrelevant code that causes compile errors.
In fb439a5 we have removed enough to compile all the relevant stabilizer functions on x86.

In fb439a5, we add code to generate random state, setpoint, and sensor inputs to the controller.
Note that for each trial, we generate these once but then run the controller 10 times.
This ensures that we are testing the integral terms in the PID controller also.
We write the output motor ratios into a text file `original_outputs.txt`.
We use a Python script to parse the and compare text outputs.

(Looking at this file, we can see that some sequences have all zero outputs.
This is to be expected, since our totally random states and setpoints mean that if the setpoint
position is below the state, the best thing to do is output zero thrust.
For the nonzero sequences, we can observe small variations from the PID integral term.)

Between e7852ac and c36e180, in each commit we make a very small modification
that brings us a little closer to proper units everywhere.
The very small changes with randomized testing were crucial --
in previous attempts, we tried to change too much in one step, and always made mistakes.

In 05e7406, we use the C preprocessor and a Python script
to resolve the numerical macros in the new controller gains into floating-point literals.
There are still some macros left, but they are mostly to reproduce the clamping of the original version
so the results don't change.
That clamping at the control/power interface is not necessary now that we are using floats.

Finally in 05e7406, in preparation for contributing back to the CF firmware,
we remove our new `motorsSetThrust` and go back to the fixed-point `motorsSetRatio`.
This is just to keep the commit smaller.

Remarks
-------

- We were not handling the feedforward acceleration command correctly.
  The manually tuned `massThrust` constant did not correctly cancel out the fixed-point units
  in the controller / power-distribution interface.
  For now, we have preserved the old behavior, but it appears that our feedforward command is being attenuated.
  Fixing this could give better tracking performance.
- Previously, the yaw and roll/pitch gains were about equal.
  This was suspicious, because the quadrotor's yaw authority is *much* weaker than its
  roll/pitch authority, so one should not attempt to track yaw as precisely.
  After fixing the units (which required introducing the torque/thrust ratio system ID parameter),
  we can see that the yaw gains in units 1/s^2 are much smaller.
- This change has not yet been tested on the hardware, but it will be soon.

  

[1] Förster, Julian. System Identification of the Crazyflie 2.0 Nano Quadrocopter. Bachelor Thesis. ETH Zürich. 2015.
