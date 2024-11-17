
#if defined __cplusplus
class GamepadMotion;

// initialize and finalize gamepad motion data

extern "C" GamepadMotion *gamepad_motion_init();
extern "C" void gamepad_motion_fini(GamepadMotion *gp);

// start, stop, and reset calibration

extern "C" void gamepad_motion_start_calibration(GamepadMotion *gp);
extern "C" void gamepad_motion_stop_calibration(GamepadMotion *gp);
extern "C" void gamepad_motion_reset_calibration(GamepadMotion *gp);

// get calibration data

extern "C" void gamepad_motion_calibration(GamepadMotion *gp, float *x, float *y, float *z);

// process raw motion (gyro and acceleration) data

// NOTE: The input should be IMU data in radians/s and m/s^2 as provided by,
// e.g., joyosc. These will be converted to degrees/s and g (1g = 9.81 m/s^2)
// as required by the algorithm, and the output will be in the same units.

extern "C" void gamepad_motion_process(GamepadMotion *gp,
  float gyroX, float gyroY, float gyroZ,
  float accelX, float accelY, float accelZ, float deltaTime);

// return the gravity direction in the controller's local space, measured in
// g; if the controller is still on a flat surface, this should be
// approximately (0, -1, 0)

extern "C" void gamepad_motion_gravity(GamepadMotion *gp, float *x, float *y, float *z);

// return calibrated rotation (x,y,z) or (x,y) degrees/s data in local, world,
// and player space; more details can be found at Jibb Smart's wiki, see:
// http://gyrowiki.jibbsmart.com/blog

// NOTE: We use a patched version of Jibb's GamepadMotion.hpp here. The
// original version seems to have left and right swapped in some cases
// (probably a simple sign error somewhere). At least that's the case with the
// gyro data provided by the SDL library (such as in joyosc). Our fix is to
// just flip the sign in the right places so that the gyro API functions
// return proper yaw/roll data (pointing left if turning or leaning left).

extern "C" void gamepad_motion_local(GamepadMotion *gp, float *x, float *y, float *z);
extern "C" void gamepad_motion_world(GamepadMotion *gp, float *x, float *y);
extern "C" void gamepad_motion_player(GamepadMotion *gp, float *x, float *y, float f);

#else

// C doesn't know about C++ classes, so we fake it
#define GamepadMotion void

extern GamepadMotion *gamepad_motion_init();
extern void gamepad_motion_fini(GamepadMotion *gp);
extern void gamepad_motion_start_calibration(GamepadMotion *gp);
extern void gamepad_motion_stop_calibration(GamepadMotion *gp);
extern void gamepad_motion_reset_calibration(GamepadMotion *gp);
extern void gamepad_motion_calibration(GamepadMotion *gp, float *x, float *y, float *z);
extern void gamepad_motion_process(GamepadMotion *gp,
  float gyroX, float gyroY, float gyroZ,
  float accelX, float accelY, float accelZ, float deltaTime);
extern void gamepad_motion_gravity(GamepadMotion *gp, float *x, float *y, float *z);
extern void gamepad_motion_local(GamepadMotion *gp, float *x, float *y, float *z);
extern void gamepad_motion_world(GamepadMotion *gp, float *x, float *y);
extern void gamepad_motion_player(GamepadMotion *gp, float *x, float *y, float f);

#endif
