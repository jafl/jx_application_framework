[![Github CI](https://github.com/jafl/jx_application_framework/actions/workflows/ci.yml/badge.svg?branch=utf8)](https://github.com/jafl/jx_application_framework/actions/workflows/ci.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?branch=utf8&project=jafl_jx_application_framework&metric=alert_status)](https://sonarcloud.io/dashboard?id=jafl_jx_application_framework&branch=utf8)

[![Total alerts](https://img.shields.io/lgtm/alerts/g/jafl/jx_application_framework.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/jafl/jx_application_framework/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/jafl/jx_application_framework.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/jafl/jx_application_framework/context:cpp)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?branch=utf8&project=jafl_jx_application_framework&metric=ncloc)](https://sonarcloud.io/dashboard?id=jafl_jx_application_framework&branch=utf8)

[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?branch=utf8&project=jafl_jx_application_framework&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=jafl_jx_application_framework&branch=utf8)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?branch=utf8&project=jafl_jx_application_framework&metric=security_rating)](https://sonarcloud.io/dashboard?id=jafl_jx_application_framework&branch=utf8)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?branch=utf8&project=jafl_jx_application_framework&metric=vulnerabilities)](https://sonarcloud.io/dashboard?id=jafl_jx_application_framework&branch=utf8)

libjcore: [![Coverage](https://codecov.io/gh/jafl/jx_application_framework/branch/utf8/graph/badge.svg)](https://codecov.io/gh/jafl/jx_application_framework)

# JX Application Framework

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

To build the libraries and programs, simply run:

    ./configure
    make

If you are new to the JX Application Framework, then it is important to
understand that it is not a program that you can run.  Rather, it is a set
of classes that provide a foundation on which you can build a program with
a graphical interface.  If you are familiar with either MicroSoft Visual
Studio or MetroWerks CodeWarrior, then the equivalent components are as
follows:

New Planet Software       | MicroSoft       | MetroWerks
-------------------       | ---------       |  ----------
Code Crusader/Code Medic  | Visual Studio   |  CodeWarrior
`jx_project_wizard`       | Project Wizards |  Project Wizards
JX Application Framework  | MFC             |  PowerPlant
`JX/tutorials/`           | MSDN            |  Sample code
`JX/libjx/testui/`        | MSDN            |  Sample code

In this directory, you will find:

```
README.md            This file
INSTALL.md           Instructions for compiling and installing this package
LICENSE              License information
CHANGES.md           Describes changes from previous versions
APPS.md              General FAQ for programs built using JX

tutorial             Tutorial source code demonstrating JX library
doc                  Documentation for JX library
lib                  Links to static and shared libraries

include
  make               Include files for Makefiles
  jcore              Link to header files for JCore
  jx                 Link to header files for JX

libjcore
  code               JCore source
  test               Unit tests

libjx
  code               JX source
  test               Unit tests
  testui             Sample application demonstrating JX library

tools
  compile_jstrings   Compiles string database files for use with JX
  jx_memory_debugger Graphical display and inspection of memory usage
  jx_project_wizard  Wizard to create skeleton JX application
                     and project file for use in Code Crusader 
  jxlayout           Graphical window layout editor
  makemake           Builds Makefile from Make.files and Make.header
  webgif             Utility program for working with gifs

ACE
  test               Sample source for using ACE library

misc
  lldb-include       lldb includes for systems that do not have them
  win32              Unfinished port to Windows
```

Please read the LICENSE file for the conditions under which the source is
provided.

The code has been thoroughly tested on Linux and MacOS.  Bizzare behavior
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

## FAQ

### General problems

#### Can I get a pre-compiled version of JX?

Only via homebrew.  We gave up trying to distribute other binaries a
long time ago.  Everybody has a different version of the compiler and/or
the required system libraries, so it causes more problems than it solves.


#### How do I set environment variables?

At the command line type:

    echo $SHELL

This will tell you if you are using `/bin/tcsh` or `/bin/bash` as your shell.

If you are using `/bin/tcsh`:

    setenv VARIABLE VALUE

If you append this to the file `~/.cshrc`, the variable will be set
automatically every time you log in.

If you are using `/bin/bash`:

    export VARIABLE=VALUE

If you append this to the file `~/.bashrc`, the variable will be set
automatically every time you log in.


#### `makemake` hangs.  How do I fix this?

Run it from the shell, not from another program (e.g. Midnight Commander).

Or you can patch it by appending the following arguments to the call to
`JProcess::Create()`: `kJIgnoreConnection, NULL, kJTossOutput, NULL,
kJAttachToFromFD, NULL`


#### It gives me the error:  "'touch' has both : and :: entries."  How do I
fix this?

Remove all old copies of `makemake` from your system.


#### Why does it complain that it can't find the **ACE** header files?

You need to set the environment variable `ACE_ROOT`, as explained in the
instructions printed when you run `make` in the top level directory.


#### Why does it complain that it can't find `Xlib.h`?

You need to create a symbolic link `/usr/include/X1`1 that points to the
directory containing the X11 header files on your system.  This is often
`/usr/X11R6/include`.

If you do not have root access and your system administrator doesn't want
to create the link, then uncomment `J_X11_INCLUDE_DIR` and set it to point
to the directory containing the X11 header files.


#### When I compile, I get warnings about implicit declarations of functions
in **ACE** header files.  How do I fix this?

Comment out `J_DIE_ON_WARNING` in `include/make/jx_config`.


#### When I compile, I get an error "undefined reference to 'CMSG_DATA'".
How do I get around this?

Uncomment `J_FORCE_ACE_STATIC` in `include/make/jx_config`.  Then delete
`ACE/ACE_wrappers/ace/libACE.so` and run make again.  Please notify us so
we can update the configuration for your system.


#### When I link, it complains that it can't find `libXpm`.  How do I fix this?

Uncomment `JX_INCLUDE_LIBXPM` in `include/make/jx_config`.


#### How do I avoid stripping the shared libraries?

Change `J_STRIP_DEBUG` to `echo` in `include/make/jx_config`.


#### How do I avoid installing the binaries when I build?

Run `make J_WANT_INSTALL=0 *target*`


#### Do I have to use the version of **ACE** included with **JX**?

No.  **JX** uses only the elementary functionality provided by **ACE**, and
since this is very unlikely to change, you can safely use newer versions.


#### How do I debug window layouts that adjust to translated text?

Hold down the Control, Meta, and Hyper keys while the window is being
created.  Debugging horizontal expansion is the default.  Hold down shift
to debug vertical expansion.  The color progression is:
* blue
* red
* yellow
* light blue
* white
* cyan
