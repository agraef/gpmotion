# gpmotion: process motion data with Pd

This package includes the gpmotion external and some examples which show how to work with gamepad or smartphone motion (IMU = Inertial measurement unit, a.k.a gyroscope) data in Pd.

The package uses pdlibbuilder, so building the gpmotion external should be simple if you have gcc or clang as well as Pd installed; just run `make` and you should be set. Installing the external along with the documentation and examples can be done with `make install`, and and `make clean` gets rid of the compiled artifacts.

The included examples are controller-test.pd (a simple test patch which doesn't require the gpmotion external and thus can be used without running `make`), gpmotion-help.pd (the help patch for the gpmotion external), and gpmotion-help.pd (a more elaborate version of the help patch).

The patches take OSC (not gamepad) input in the format provided by [joyosc](https://github.com/danomatika/joyosc), so you need to have that installed as well (run it as `joyosc -s` so that it reports the sensor data). Alternatively, you can also use [TouchOSC](https://hexler.net/touchosc) with the provided joyosc.tosc template.

Note that the gpmotion external requires a device that generates *both* acceleration and gyroscope (angular motion) data. Most PlayStation- and Switch-compatible gamepads will provide this, but some may need to be switched into a special mode to enable this (the user guide that comes with your controller should tell you how to do this). Some mobile devices may provide acceleration but no angular motion data; these will *not* work with the gpmotion object. When in doubt, use the controller-test.pd patch to check the raw sensor data of the device. If the `accel` or `gyro` data shows nothing but zero values then that device won't be suitable to be used with the gpmotion object.

More information can be found in the gyroscope.pdf document contained in the package.