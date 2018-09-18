## libgdiplus: An Open Source implementation of the GDI+ API.

This is part of the [Mono project](http://www.mono-project.com/).

Build status:

|    |   |
|----|---|
| Travis (Linux/OSX) | [![Travis Build Status](https://travis-ci.org/mono/libgdiplus.svg?branch=master)](https://travis-ci.org/mono/libgdiplus)|
| Jenkins (Linux)    | [![Jenkins Linux Build Status](https://jenkins.mono-project.com/job/test-libgdiplus-mainline/badge/icon)](https://jenkins.mono-project.com/job/test-libgdiplus-mainline) |
| Jenkins (Windows)  | [![Jenkins Windows Build Status](https://jenkins.mono-project.com/job/test-libgdiplus-mainline-windows/badge/icon)](https://jenkins.mono-project.com/job/test-libgdiplus-mainline-windows) |

### Requirements:

This requires the libraries used by the Cairo vector graphics library to build (freetype2, fontconfig, Xft2 and libpng).

On **OSX** you can use [Homebrew](https://brew.sh/) to install the dependencies:

	brew install glib cairo libexif libjpeg giflib libtiff autoconf libtool automake pango pkg-config
	brew link gettext --force

On **Debian-based Linux distributions** you can use `apt-get` to install the dependencies:

	sudo apt-get install libgif-dev autoconf libtool automake build-essential gettext libglib2.0-dev libcairo2-dev libtiff-dev libexif-dev

On **Windows** you can use [Vcpkg](https://github.com/Microsoft/vcpkg) to install the dependencies. Run the following commands from the root of the repository from an admin command prompt:

	bootstrap-vcpkg.bat
	vcpkg.exe integrate install
	vcpkg.exe install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango --triplet x86-windows
	vcpkg.exe install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango --triplet x64-windows

### Build instructions

To build on **OSX** without X11:

	./autogen.sh --without-x11 --prefix=YOUR_PREFIX
	make

To build on **OSX with X11** (e.g. from XQuartz):

	PKG_CONFIG_PATH=/opt/X11/lib/pkgconfig ./autogen.sh --prefix=YOUR_PREFIX
	make

To build on **Linux**:

	./autogen.sh --prefix=YOUR_PREFIX
	make

To build on **Windows**, open `libgdiplus.sln`.

### Running the unit tests

Run the following command from the root of the repository:

	make check

To run the tests with Clang sanitizers, run the following command from the root of the repository:

	./autogen.sh --enable-asan
	make check

### Code coverage

Code coverage stats are generated with `lcov`. You can use [Homebrew](https://brew.sh/) on **OSX** to install the dependencies:

	brew install lcov

To run the tests with code coverage, run the following commands from the root of the repository:

	./autogen.sh --enable-coverage
	make check
	lcov --capture --directory src --output-file coverage.info
	genhtml coverage.info --output-directory coverage

To view the coverage report, navigate to the `coverage` directory in the root of the repository and open `index.html`.

### Installing libgdiplus

Run the following command from the root of the repository:

	make install

### Optional build options

	--with-pango

	This builds libgdiplus using Pango to render (measure and draw) 
	all of it's text. This requires Pango version 1.38 (or later).
