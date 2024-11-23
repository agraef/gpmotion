# gpmotion: process motion data with Pd

This package includes the gpmotion external and some examples which show how to work with gamepad or smartphone motion (IMU = Inertial measurement unit, a.k.a gyroscope) data in Pd.

## Installation

The package uses pdlibbuilder, so building the gpmotion external from source should be simple if you have gcc or clang as well as Pd installed; just run `make` and you should be set. (The build requires a fairly recent version of gcc or clang which supports the C++17 standard.) As usual, installing the external along with the documentation and examples can be done with `make install`, and and `make clean` gets rid of the compiled artifacts.

Binary packages are available from [GitHub](https://github.com/agraef/gpmotion/releases) (as usual, the latest builds can be found on the [Actions](https://github.com/agraef/gpmotion/actions) tab) and from [Deken](http://deken.puredata.info/library/gpmotion). These can be used with both vanilla [Pd](http://msp.ucsd.edu/software.html) and [Purr Data](https://agraef.github.io/purr-data/), but the latter requires that you rename the included `.so` or `.dll` file to  `gpmotion.pd_linux` (Linux/Ubuntu), `gpmotion.pd_darwin` (macOS) or `gpmotion.dll` (Windows), respectively.

**NOTE:** The `gpmotion-*-pd32` packages are for single precision builds of Pd, which is the version that you should get (unless you built Pd yourself with double precision support). All builds are for 64 bit systems, the macos packages will work on both Intel and Apple Silicon Macintosh computers. Also, these are unsigned packages, so on macOS you'll have to run through the usual incantations to refute Gatekeeper. Run this in the terminal, in the directory containing the `gpmotion.pd_darwin` file:

~~~shell
sudo xattr -d com.apple.quarantine gpmotion.pd_darwin
~~~

**NOTE:** On Linux, there's built-in functionality to have the touchpad of a PS4(-compatible) controller like the DualShock 4 function as a mouse. This gets in the way if you'd like to use the touchpad (in addition to the motion sensors) in a Pd patch. Therefore we have included a 72-ds4tm.rules file in the etc subdirectory which disables the unwanted mouse functionality, see this [reddit comment](https://www.reddit.com/r/Fedora/comments/15sri25/comment/jwz9rgi/) for details. This is a udev rules files to be copied to the /etc/udev/rules.d directory on your Linux system.

## Usage

The included examples are controller-test.pd (a simple test patch which doesn't require the gpmotion external and thus can be used without running `make`), gpmotion-help.pd (the help patch for the gpmotion external), as well as gpmotion-demo.pd and gpmotion-test.pd, two more advanced versions of the help patch. The latter also features fancy radar gui objects for visualizing joysticks, touchpad, and world/player space motion; these are programmed in Lua and thus require [pd-lua](https://agraef.github.io/pd-lua/) to work.

The patches take [OSC](https://opensoundcontrol.stanford.edu/) (not gamepad) input in the format provided by [joyosc](https://github.com/danomatika/joyosc), so you need to have that installed as well (run it as `joyosc -s` so that it reports the sensor data; at the time of this writing, you need the very latest git version from joyosc's [develop](https://github.com/danomatika/joyosc/tree/develop) branch to make that work). Alternatively, you can also use [TouchOSC](https://hexler.net/touchosc) with the provided joyosc.tosc template.

More information can be found in the gyroscope.pdf document contained in the package.