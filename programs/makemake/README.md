Important note
==============

The directory containing makemake must be on your execution path in order
for makemake to work correctly.

makemake
========

makemake was written to alleviate the pain of declaring #include
dependencies in make by generating a Makefile from a list of source files.

More specifically, given the two files Make.header and Make.files, makemake
parses the files specified in Make.files to calculate their dependency
lists and then generates a Makefile compatible with GNU's version of make.

Make.header is simply copied to become the first part of the Makefile.  It
should contain all the variable definitions and special targets.  (makemake
automatically generates the targets all, tidy (double colon), clean (double
colon), checksyntax (see makecheck below), touch (see maketouch below), and
jdepend (internal use only).)  In particular, LINKER must be defined to be
the name of the program to use during linking, and DEPENDFLAGS must be set
to contain all the compiler directives so that jdepend will work correctly.
(LINKER is usually just the same as the name of the compiler, but it needs
to be mcc in order to use Mathematica's MathLink package.)

Here is a simple example of a Make.header file:

    # Useful directories

    MYCODEDIR := .

    # Directories to search for header files

    SEARCHDIRS := -I- -I${MYCODEDIR}

    # makemake variables

    LINKER       := gcc
    DEPENDFLAGS  := -g -Wall -Werror ${SEARCHDIRS}

    # make variables

    CC       := gcc
    CXX      := g++
    CCC      := g++
    CPPFLAGS  = ${DEPENDFLAGS}

If you include a line "### makemake inserts here ###" somewhere in
Make.header, then the targets that makemake generates will be inserted
there instead of being appended at the end.

Make.files contains a simple list of all the files required to build each
target as follows:

    @<name of first target>
    <first object file>
    <second object file>
    ...
    @<name of second target>
    ...

The object files should not contain a suffix, because makemake appends the
appropriate suffixes automatically.  (e.g. .c, .cc, .o, etc.)  The source
file suffix is .c by default, but can be set with the -suffix option.  The
current suffix can be overridden by placing the desired suffix in front of
the source name as follows:

    -suffix .cc
    @myprog
    .c myprog
    JString

The first line sets the default suffix to .cc.  The next line specifies the
name of the make target.  The third line tells makemake that the object
file myprog.o is required in order to build myprog, and that the source
file is called myprog.c.  The last line tells makemake that the object file
JString.o is also required in order to build myprog, and that the source
file is called JString.cc.

You can also specify the suffix for the output file by including it between
the source suffix and the file name:

    @myprog
    .java .class myprog

Note that this is not usually necessary since makemake is reasonably
intelligent about guessing the correct output suffix.

makemake correctly calculates dependencies for lex, flex, yacc, and bison
files.  Simply specify the .l or .y suffix.  Dependencies on libraries can
be included by specifying the .a or .so suffix.  (make is not powerful
enough to handle .so.<vers>, so create a symbolic link to get a pure .so
file.)  Libraries must also be listed separately in the standard make
variable LOADLIBES so the linker will pick the correct type.  Dependencies
on precompiled files can also be included by specifying the .o suffix.
This is useful if some files have to be compiled in a completely different
way, e.g. using a different Makefile. makemake recognizes .a, .so, .o,
.class, and .jar as special suffixes and does not try to calculate their
dependencies.  You can specify additional suffixes that should not be
parsed with the --no-parse command line option.

The -prefix option sets the prefix of all subsequent source files.  This is
useful if a group of files is in a different directory from the Makefile.
For convenience, the prefix is cleared when a new target is encountered.
As usual, comments can be included in Make.files by starting the line with
a hash (#).

If no objects files are listed for a target, it is assumed that they are
the same as those for the next target in the file.  This is especially
convenient for building several different versions of the same library.
(e.g. static .a and shared .so versions)

The command line options for makemake are as follows:

  -h                prints help
  -hf               <header file name>  - default Make.header
  -if               <project file name> - default Make.files
  -of               <output file name>  - default Makefile
  --obj-dir         <variable name>     - specifies directory for all .o files
  --no-std-inc      exclude dependencies on files in /usr/include
  --assume-autogen  assume unfound "..." files reside in includer's directory
  --check           only rebuild output file if input files are newer
  --choose          interactively choose the targets
  --make-name       <make binary> - name of GNU make binary

The following options can be used multiple times:

  --define      <code> - make code to prepend to Make.header
  --search-path <path> - add to paths to search after . for input files
  --suffix-map  .x.y   - file.y results from compiling file.x
  --no-parse    .x.y.z - specifies suffixes of files which should not be parsed

The --choose option prints a list of the targets found in Make.files, and
lets you choose which ones to include in the final Makefile.  Another way
to specify a particular subset of targets is to include their names after
the other options.

As mentioned above, your own special targets should be included in
Make.header.  Some targets (e.g. TAGS used for the etags program) require a
list of all the source files.  makemake provides this in the variable
called MM_ALL_SOURCES.  Thus, to define the TAGS target, simply include the
following in your Make.header:

    .PHONY : TAGS
    TAGS: ${MM_ALL_SOURCES}
        etags ${MM_ALL_SOURCES}

makecheck
=========

Sometimes it is helpful to be able to check a single source file after
modifying it.  This provides instant feedback on typos and such.  Of
course, one would prefer not to have to run make and wait while other files
are recompiled first.  This can be done by running make directly, but one
has to include the path exactly the way it is specified in the Makefile.
makecheck was written to solve this problem.  Once the Makefile is built,
simply type "makecheck MyClass.cc" to recompile only MyClass.cc.  makecheck
finds the correct path for you.

maketouch
=========

Sometimes it is necessary to use compile-time flags to include or exclude
certain pieces of code.  (One should obviously try to minimize the need for
this, but NDEBUG, which turns off assert(), will always be there.)  In such
cases, one would prefer not to have to recompile everything (i.e. make
clean) when a flag that only affects a few source files is changed.
maketouch was written to solve this problem.  It works in conjunction
with makemake's special touch target to insure that only the files that use
the flag are actually recompiled.

Since makemake does most of the work automatically, maketouch is very easy
to use.  Simply type "maketouch compile_flag" to touch all the source and
header files that include "compile_flag".  If the program depends on
libraries that use the same "compile_flag", simply add the paths to each
library's Makefile to the TOUCHPATHS variable.  makemake will automatically
run maketouch in each of these directories.  If you need to do anything
more complicated, you can define extra code for the touch target in
Make.header as follows:

    .PHONY : touch
    touch::
        <your code here>
