
#include "GamepadMotion.hpp"
#include "motion.h"

extern "C" GamepadMotion *gamepad_motion_init()
{
  return new GamepadMotion();
}

extern "C" void gamepad_motion_fini(GamepadMotion *gp)
{
  delete gp;
}

extern "C" void gamepad_motion_start_calibration(GamepadMotion *gp)
{
  gp->StartContinuousCalibration();
}

extern "C" void gamepad_motion_stop_calibration(GamepadMotion *gp)
{
  gp->PauseContinuousCalibration();
}

extern "C" void gamepad_motion_reset_calibration(GamepadMotion *gp)
{
  gp->ResetContinuousCalibration();
}

extern "C" void gamepad_motion_process(GamepadMotion *gp,
  float gyroX, float gyroY, float gyroZ,
  float accelX, float accelY, float accelZ, float deltaTime)
{
  // we need to normalize acceleration values (9.81 m/s^2 = 1g) and convert
  // gyro values to degrees/s for Jibb's algorithm
  gp->ProcessMotion(gyroX/M_PI*180.0, gyroY/M_PI*180.0, gyroZ/M_PI*180.0,
    accelX/9.81, accelY/9.81, accelZ/9.81, deltaTime);
}

extern "C" void gamepad_motion_gravity(GamepadMotion *gp, float *x, float *y, float *z)
{
  gp->GetGravity(*x, *y, *z);
}

extern "C" void gamepad_motion_local(GamepadMotion *gp, float *x, float *y, float *z)
{
  gp->GetCalibratedGyro(*x, *y, *z);
}

extern "C" void gamepad_motion_world(GamepadMotion *gp, float *x, float *y)
{
  gp->GetWorldSpaceGyro(*x, *y);
}

extern "C" void gamepad_motion_player(GamepadMotion *gp, float *x, float *y, float f)
{
  gp->GetPlayerSpaceGyro(*x, *y, f);
}
