## libgdiplus: An Open Source implementation of the GDI+ API.

This is part of the [Mono project](http://www.mono-project.com/).

### Requirements:

This requires the libraries used by the Cairo vector graphics library to build (freetype2, fontconfig, Xft2 and libpng).

On OSX you can use Homebrew to install the dependencies:

	brew install glib cairo libexif libjpeg giflib libtiff autoconf libtool automake


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

### Running the unit tests

Run the following command from the root of the repository:

	make check

### Installing libgdiplus

Run the following command from the root of the repository:

	make install

### Optional build options [UNSUPPORTED]

	--with-pango

	This builds libgdiplus using Pango to render (measure and draw) 
	all of it's text. This requires Pango version 1.10 (or later).
