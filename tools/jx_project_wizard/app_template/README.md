# <Program>

Add a description...

The easiest way to install from the binary package is to run `sudo ./install`

Otherwise, the program (<Binary>) should be placed in a directory that is on your execution path.  `/usr/bin` or `~/bin` are usually good choices.  Copy the libraries in `lib/` to a directory on your `LD_LIBRARY_PATH`.


## Building from source

1. Install the [JX Application Framework](https://github.com/jafl/jx_application_framework),  and set the `JX_ROOT` environment variable to point to its `include` directory.
1. `./configure`
1. `make`


## Installing a binary

For macOS, the easiest way to install is via [Homebrew](https://brew.sh):

    brew install --cask xquartz
    brew tap ...
    brew install .../<Binary>

For all other systems, download a package from:

* https://github.com/.../<Binary>/releases

If you download the tar, unpack it, cd into the directory, and run `sudo ./install`.

Otherwise, put the program (`<Binary>`) in a directory that is on your execution path.  `~/bin` is typically good choices.

### Requirements

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


## FAQ

For general questions, refer to the [Programs FAQ](https://github.com/jafl/jx_application_framework/blob/master/APPS.md).
