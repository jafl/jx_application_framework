Compiling
=========

`./configure; make`

Notes
-----

If you want the binaries installed somewhere other than `~/bin` (or
`./bin`, if `~/bin` doesn't exist), set the environment variable
`JX_INSTALL_ROOT` to the desired directory.

To compile the tutorials or test suites, go to the directory and run first
makemake and then make.

If `fdesign` (the graphical layout tool) is not installed, you can download
the source from http://xforms-toolkit.org.

To keep the Makefiles smaller, it is recommended to set the
`J_MAKEMAKE_IGNORE_PATTERN` environment variable to `^ace/$`.

Since ACE is huge, only one version of the library is built, if it has to
be done from source.  If you want the other version as well, go to the ACE
directory and type `make shared` or `make static` after you have built
everything else.

Do not add `./lib` to your `LD_LIBRARY_PATH`.  This will cause conflicts
with the shared libraries used by Code Crusader, Code Medic, etc.

Creating your own programs
==========================

Run jx_project_wizard to get a skeleton program.

For debugging, it is a good idea to set the following environment variables:

    setenv JMM_INITIALIZE   "default"
    setenv JMM_SHRED        "default"

Check libjcore/code/JMemoryManager.cpp for more information.
