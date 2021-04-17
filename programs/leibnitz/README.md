Installing
==========

The easiest way to install from the binary package is to run `sudo ./install`

Otherwise, the program (leibnitz) should be placed in a directory that is on your execution path.  `/usr/bin` or `~/bin` are usually good choices.  Copy the libraries in `lib/` to a directory on your `LD_LIBRARY_PATH`.


Requirements
============

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.

You also need to `brew install ace icu4c pcre gd`.


Notes
=====

Leibnitz provides a calculator and the ability to plot functions.  The on-line help explains all the features of the program.

http://sourceforge.net/p/leibnitz/


FAQ
===

For general questions, refer to the [Programs FAQ](../README.md).

Usage
-----

### How do I clear the expression and start over after pressing '=' without using the mouse?

Use Meta-A. (Select All on the Edit menu)  Typing will replace the selection.


### How do I avoid saving the contents of the tape between sessions?

Use the Close button instead of the Quit menu item.


### Why did you call it Leibnitz?

The program is advocating a better way to display calculations, just like Leibnitz advocated a notation for Calculus that proved superior to what Newton had invented.
