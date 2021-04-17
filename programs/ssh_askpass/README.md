Installing
==========

The easiest way to install from the binary package is to run `sudo ./install`

Otherwise, the program (nps-ssh-askpass) should be placed in a directory that is on your execution path.  `/usr/bin` or `~/bin` are usually good choices.  Copy the libraries in `lib/` to a directory on your `LD_LIBRARY_PATH`.


Requirements
============

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.

You also need to `brew install ace icu4c pcre gd`.


Notes
=====

nps-ssh-askpass does exactly the same job as x11-ssh-askpass, but it's not as hard to compile on MacOS, because it doesn't require xmkmf.

http://sourceforge.net/p/nps-ssh-askpass/


FAQ
===

For general questions, refer to the [Programs FAQ](../README.md).
