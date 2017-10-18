Preparation
===========

Decide where you want the JX directory to reside.  Absolute paths are
compiled into some of the utility programs, so you will have to rebuild
everything if you move JX to a different location after building it.

Run ./configure in the JX directory.  This will try to install all the
reqired packages on your system.

If your system is unrecognized, you will need to figure out how to install
the required tools and libraries.  Start with g++, make, flex, bison, and
the basic X11 libraries and headers.

If it is not possible to install ACE from a package, the build will
download ACE automatically.  If you download and unpack ACE yourself
(http://www.cs.wustl.edu/~schmidt/ACE.html), then the source may need to be
patched.  In the ACE directory, run patch_ace.

If fdesign (the graphical layout tool) is not installed, you can download
the source from http://xforms-toolkit.org.

To keep the Makefiles smaller, it is recommended to set the following
environment variable:

    setenv J_MAKEMAKE_IGNORE_PATTERN '^ace/$'
    export J_MAKEMAKE_IGNORE_PATTERN='^ace/$'


Compiling
=========

If you want the binaries installed somewhere other than ~/bin (or JX/bin,
if ~/bin doesn't exist), set the environment variable JX_INSTALL_ROOT to
the desired directory.

  Use "setenv JX_INSTALL_ROOT value" at the top of ~/.cshrc if you use csh
  or "export JX_INSTALL_ROOT=value" at the top of ~/.bashrc if you use
  bash.

This directory (JX/bin, ~/bin, or $JX_INSTALL_ROOT), must be on your
execution path.

  Use "set path = ( new_path $path )" at the top of ~/.cshrc if you use csh
  or "export PATH=new_path:$PATH" at the top of ~/.bashrc if you use bash.

Do not add JX/lib to your LD_LIBRARY_PATH.  This will cause conflicts
with the shared libraries used by Code Crusader, Code Medic, etc.

Run "make" and following the instructions for setting ACE_ROOT and
compiling the libraries and utility programs.  This will compile everything
and install the programs.  You can ignore any errors that make ignores.

  You can avoid installing the binaries by setting the environment variable
  J_WANT_INSTALL equal to 0.

  Everything in JX will be built, including any 3rd party libraries and
  programs that you unpack after the main distribution.

Since ACE is huge, only one version of the library is built, if it has to
be done from source.  If you want the other version as well, go to the ACE
directory and type "make shared" or "make static" after you have built
everything else.

If your system is not already supported, create another entry in the
Makefile by copying one of the existing ones and modifying it as follows:

  Start by assuming that jMissingProto_empty.h is appropriate. If you get
  compile errors, create a new one and fill it with whatever is necessary.

  Create a new file in include/make/sys/ by copying template_g++ and
  editing the "Adjustable Parameters" section.

  If you cannot find a suitable pair of ACE configuration files, contact
  the developers.  (http://www.cs.wustl.edu/~schmidt/ACE.html)

  Once you get the entire distribution to compile, contact me so I can add
  your patches to the distribution so you won't have to do it again.


Additional configuration
========================

It is a good idea to set the following environment variables:

    setenv JMM_INITIALIZE   "default"
    setenv JMM_SHRED        "default"

Check libjcore/code/JMemoryManager.cc for more information.


Compiling the other sample programs
===================================

To compile the tutorials or test suites, go to the directory and run first
makemake and then make.


Creating your own programs
==========================

Run jx_project_wizard to get a skeleton program.
