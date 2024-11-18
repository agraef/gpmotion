# gpmotion: process motion data with Pd

This package includes the gpmotion external and some examples which show how to work with gamepad or smartphone motion (IMU = Inertial measurement unit, a.k.a gyroscope) data.

The package uses pdlibbuilder, so building the gpmotion external should be simple if you have gcc or clang as well as Pd installed; just run `make` and you should be set. Installing the external along with the documentation and examples can be done with `make install`, and and `make clean` gets rid of the compiled artifacts.

The included examples are controller-test.pd (a simple test patch which doesn't require the gpmotion external and thus can be used without running `make`) and gpmotion-help.pd (a more elaborate help patch for the gpmotion external).

More information can be found in the gyroscope.pdf document contained in the package.