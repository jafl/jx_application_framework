Building multiple versions of a program from a single project
=============================================================

This page explains how to build multiple versions of a program (e.g. debug and release) or library (e.g. static and shared) from a single Code Crusader project.

Code Crusader does not provide a completely general interface for building several different programs or libraries from a single project because this would complicate the user interface without providing any significant savings.  Shared code should be placed in libraries, and Make.header can be a symbolic link if one wants several projects to use the same settings.

-----

You must invoke `makemake --obj-dir BIN_DIR` instead of only `makemake` in the Make Settings dialog.

Remember to use tabs in front of the commands if you copy the text directly off this web page.

-----

To build debug and release versions of a program with source code in directories `${SRC_DIR_1}` and `${SRC_DIR_2}`:

    CXX      := g++
    CPPFLAGS  = ...
    CXXFLAGS  = ...

    SRC_DIR_1 := ...
    SRC_DIR_2 := ...

    ifeq (${debug},1)
      BIN_DIR  := ./obj_debug
      CXXFLAGS += -g
    else
      BIN_DIR  := ./obj_release
      CXXFLAGS += -DNDEBUG
    endif

    # Make sure that the directory exists.
    TEMP := ${shell test -d ${BIN_DIR} || mkdir ${BIN_DIR}}

    # Tell make to search each directory
    VPATH := ${SRC_DIR_1}:${SRC_DIR_2}

    # You can modify this to use a suffix other than .cc
    ${BIN_DIR}/%.o : %.cc
        ${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $&lt; -o $@

    # Build the requested version of the program.
    ifeq (${debug},1)
      default: <i>program_name</i>_debug
    else
      default: <i>program_name</i>
    endif

    tidy::
        @${RM} -r ./obj_*

In the Project Configuration dialog, specify the target name as `*program_name*, *program_name*_debug`.  (Replace `*program_name*` with the name of your program.)

To build the program, invoke `make debug=X` with X replaced by either 0 or 1.

-----

To build static and shared versions of a library with source code in directories `${SRC_DIR_1}` and `${SRC_DIR_2}`:

    CXX      := g++
    CPPFLAGS  = ...
    CXXFLAGS  = ...

    SRC_DIR_1 := ...
    SRC_DIR_2 := ...

    ifeq (${shared}${debug},11)
      BIN_DIR  := ./obj_shared_debug
      CXXFLAGS += -fPIC -g
    else
    ifeq (${shared}${debug},10)
      BIN_DIR  := ./obj_shared_release
      CXXFLAGS += -fPIC -DNDEBUG
    else
    ifeq (${shared}${debug},01)
      BIN_DIR  := ./obj_static_debug
      CXXFLAGS += -g
    else
      BIN_DIR  := ./obj_static_release
      CXXFLAGS += -DNDEBUG
    endif
    endif
    endif

    # Make sure that the directory exists.
    TEMP := ${shell test -d ${BIN_DIR} || mkdir ${BIN_DIR}}

    # Tell make to search each directory
    VPATH := ${SRC_DIR_1}:${SRC_DIR_2}

    # You can modify this to use a suffix other than .cc
    ${BIN_DIR}/%.o : %.cc
        ${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $&lt; -o $@

    default:
      ifeq (${build_shared}${debug},11)
        @${MAKE} shared=1 <i>library_name</i>_debug.so
      endif
      ifeq (${build_shared}${debug},10)
        @${MAKE} shared=1 <i>library_name</i>.so
      endif
      ifeq (${build_static}${debug},11)
        @${MAKE} shared=0 <i>library_name</i>_debug.a
      endif
      ifeq (${build_static}${debug},10)
        @${MAKE} shared=0 <i>library_name</i>.a
      endif

    tidy::
        @${RM} -r ./obj_*

In the Project Configuration dialog, specify the target name as `*library_name*.so, *library_name*_debug.so, *library_name*.a, *library_name*_debug.a`.  (Replace `*library_name*` with the name of your library.)

To build the library, invoke `make debug=X build_shared=Y build_static=Z` with X, Y, and Z each replaced by either 0 or 1.

It is left as an exercise to the reader to figure out how to link to the appropriate version of the library.  (Hint:  Modify `LOADLIBES`.)

-----

Aaron Lefohn submitted the following instructions for building multiple versions of the same program on multiple architectures that all share a single file system:

1. Add the following aliases to your `.cshrc` (if you're using tcsh/csh)

        alias fast    setenv OPTIMIZE=true
        alias nofast  setenv OPTIMIZE=false

1. Ensure that your system automatically sets the`ARCH` variable (most do)
1. Set the target name in your Code Crusader project to `${PROG_NAME}`
1. Put the following in your `Make.header` somewhere before the rules for making the objects:

        # set the program BASE and PATH
        PROG_BASE := program_name                 (e.g. a.out)
        PROG_PATH := path_to_executable_directory (e.g. ../../bin/)

        # choose optimized or debug version of program
        ifeq (${OPTIMIZE},true)
          PROG := ${PROG_PATH}${PROG_BASE}.opt
        else
          PROG := ${PROG_PATH}${PROG_BASE}.debug
        endif

        # choose the correct architecture extension for the program name
        ifneq (,$(findstring IRIX,$(ARCH)))
          PROG_NAME := ${PROG}.sgi
        endif
        ifneq (,$(findstring AIX,$(ARCH)))
          PROG_NAME := ${PROG}.aix
        endif
        ifneq (,$(findstring Linux,$(ARCH)))
          PROG_NAME := ${PROG}.x86linux
        endif

1. You can do a similar architecture and `OPTIMIZE` detection to create the name for the unique object directory. (see above)
1. The executable is then named, for example, "a.out.debug.sgi".  This is ugly, but it is easy to write a shell script that is called a.out that does the same sort of architecture detection, reads the state of the `OPTIMIZE` variable and then calls the correct executable.  Put this script in your `~/bin/` directory and voila!

You can now type "nofast" or "fast" and then the name of your program on any platform and the correct executable will launch.  You can also build from the command line simply by typing "nofast" or "fast" and then "make".  If you want to build from within Code Crusader, just set the build command to `make OPTIMIZE=true/false -k` in the Make Settings dialog.  Don't forget that the target name in the Project Configuration dialog must be `${PROG_NAME}`.
