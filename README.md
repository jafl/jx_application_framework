This repository contains all the New Planet Software tools and applications:

* JX Application Framework
* Code Crusader - IDE
* Code Medic - Debugging UI on top of gdb / Xdebug
* Code Mill - Class generator plugin for Code Crusader
* System G - File manager with git support
* Leibnitz - Graphing calculator
* Glove - Data acquisition, manipulation, and analysis tool
* Drakon - Process manager

JX Application Framework
------------------------

JX is a full-featured C++ application framework and widget library (SDK)
for use with the X Window System. The power of JX stems from its rich
feature set and extensibility which support all aspects of professional
quality application development. The library provides a wide variety of
widgets for use in your program's windows along with useful functionality
that connects the widgets and windows to make complete, working software.
The package also includes a graphical window layout editor that lets you
quickly position and configure the widgets in each of the program's
windows. JX is built directly on top of Xlib and has been carefully
optimized for performance.

The package also includes a complete test suite that demonstrates all of
the features of the library and a set of tutorial programs that introduce
the major concepts, one at a time. This provides a rich source of sample
code, both for top level application design and for individual widget
classes. It also shows how to extend the functionality of the library by,
for example, creating custom tables or menus.

To build the libraries and programs, simply run GNU make ("make" or "gmake"
on most systems) in a command line window inside the JX directory and
follow the instructions that are printed.

If you are new to the JX Application Framework, then it is important to
understand that it is not a program that you can run.  Rather, it is a set
of classes that provide a foundation on which you can build a program with
a graphical interface.  If you are familiar with either MicroSoft Visual
Studio or MetroWerks CodeWarrior, then the equivalent components are as
follows:

```
New Planet Software         MicroSoft          MetroWerks
-------------------         ---------          ----------
Code Crusader/Code Medic    Visual Studio      CodeWarrior
jx_project_wizard           Project Wizards    Project Wizards
JX Application Framework    MFC                PowerPlant
JX/tutorials/               MSDN               Sample code
JX/libjx/test/              MSDN               Sample code
```

In this directory, you will find:

```
README               This file
INSTALL              Instructions for compiling and installing this package
FAQ                  List of Frequently Asked Questions
LICENSE              License information
CHANGES              Describes changes from previous versions

tutorial             Tutorial source code demonstrating JX library
doc                  Documentation for JX library
lib                  Links to static and shared libraries

include
  make               Include files for Makefiles
  jcore              Link to header files for JCore
  jx                 Link to header files for JX

libjcore
  code               JCore source
  test               Sample source for using JCore classes

libjx
  code               JX source
  test               Sample application demonstrating JX library

programs
  code_crusader      IDE
  code_medic         Debugger
  code_mill          C++ class generator
  compile_jstrings   Compiles string database files for use with JX
  drakon             UNIX process manager
  glove              Data analysis
  jx_memory_debugger Graphical display and inspection of memory usage
  jx_project_wizard  Wizard to create skeleton JX application
                     and project file for use in Code Crusader 
  jxlayout           Graphical window layout editor
  leibnitz           Electronic paper for calculations and graphing
  makemake           Builds Makefile from Make.files and Make.header
  mondrian           (Unfinished) Graphical layout builder
  ssh_askpass        Replacement for x11-ssh-askpass
  systemg            Graphical file manager
  webgif             Utility program for working with gifs

ACE
  test               Sample source for using ACE library

misc
  pcre-8.10          PCRE regular expression package
  png                PNG library (new libgd)
  xpm                XPM library (libXpm)
  win32              Partial port to Windows
```

Please read the LICENSE file for the conditions under which the source is
provided.

The code has been thoroughly tested on Linux and OS X.  Bizzare behavior
and crashes may be the result of broken system libraries.  We am willing to
work with you to solve such problems, so please don't hesitate to contact
us, but since we don't have access to your system, the burden of proof must
rest on you if a system call that works fine on all other systems doesn't
work for you.

It is also important to realize that we do not maintain ACE.  You are
welcome to contact us if you have problems compiling it, but the ACE
developers are more likely to be able to help, and patches should be sent
to them, not to us.  JX uses ACE because networking is horribly
UNIX-variant-dependent, and the ACE developers are much better at dealing
with this than we are.  The ACE web page is:

    http://www.cs.wustl.edu/~schmidt/ACE.html

