This is version 4.0.0 of the JX distribution.

Changes from previous versions are described in the *LibVersion.h files in
the include file directories.

Changes to the build system
===========================

4.2.0
-----

* Shortened J_COMPILER_DEPEND_FLAGS to J_DEPEND_FLAGS

4.0.0
-----

* Moved all system logic into `./configure` so the system name does not
  have to be passed to `make`.
* Removed source for all embedded libraries.  Switched to (now reliably
  available) pre-compiled libraries, installed by the native package
  manager.  The new `./configure` script handles this for known systems.
* Removed J_COMPILE_DEBUG flag.  To build without debugging, create
  the file 0-RELEASE with content: 1
* Removed support for shared libraries.  RAM is plentiful.  This eliminates
  the following make variables: J_BUILD_SHARED_LIB, J_SHARED_LIB_SUFFIX,
  J_SHARED_LIB_LINK_OPTION, J_NEED_SEPARATE_O_FILES, J_BEGIN_LINK_STATIC,
  J_END_LINK_STATIC, RPM_LIB_PATH.  Removed J_SYS_LIBS.  J_STD_LIBS no
  longer includes libjcore and libjx, since these should be in LIB_DEPS.
* Removed obsolete J_MAN_SECTION_VIA_DASH_S.
* Completely restructured the build system to support Homebrew releases.
* Renamed _J_OSX to _J_MACOS

2.6.0
-----

* Removed obsolete build targets and build config.
* Cleaned up application builds by adding targets in `include/make/jx_targets`.
* Changed `J_HAS_GD` to `J_HAS_JPEG`.  xmp, gif, png are now widely available.
* Removed crypto, regex, xpm from misc directory.
* Modified definition of `J_STRING_FILES` to allow application to override
  library strings.

2.5.0
-----

* Added cygwin32 build target.
* Added Visual C++ workspace and project files for `libjcore`.
* Merged `libjtree` and `libjtoolbar` into `libjcore` and `libjx`.
* Fixed build system so finished binaries are copied instead of moved.
  This saves time if build is run multiple times, e.g., during debugging.
* Collapsed `J_HAS_GIF` and `J_HAS_PNG` into single `J_HAS_GD` build flag.

2.1.0
-----

* In order to be compatible with the binaries for Code Crusader, Code
  Medic, etc., `LD_LIBRARY_PATH` must not include `JX-2.1.0/lib`.  The `-R`
  link option is now used instead.
* `ACE_ROOT` no longer needs to be defined.

2.0.0
-----

* All `Make.headers` should stop defining `TOUCHHEADERS`.  All programs
  should define touch as a double colon target to run maketouch for the
  libraries on which they depend.
* Defined `J_STD_LIBS` to include `libjtoolbar`, `libjtree`, `libjx`, `libjcore`,
  and `${J_SYS_LIBS}`.  These are the libraries that all programs need.
* All `Make.headers` should remove `${ACE_ROOT}`, `${J_X11_INCLUDE_DIR}`, and
  `${JX_ROOT}/include/menu_image` from `SEARCHDIRS` because `jx_config`
  automatically appends them.  As a result of this change, `SEARCHDIRS` can
  now be defined with `:=`.
* Renamed `include/menu_image` to `include/image` and `libjx/menu_image` to
  `libjx/image`.
* Each file `include/make/*_constants` must use `+=` to add the library's
  string data files to `J_STRING_FILES` so applications can compile all the
  required strings into one database file.
* `jxlayout` now uses a more convenient method of specifying sizing
  options.  Use the `convert_gravity` script to convert your existing `.fd`
  files.
* `jxlayout` generates a database of all the strings used by each layout
  and generates code that uses `JStringManager`.
* If a target ends with `.so`, `makemake` automatically includes the link option
  required to produce a shared library.  `${J_SHARED_LIB_LINK_OPTION}` should
  no longer be included in `LDFLAGS` in library `Make.header` files.
* Renamed `libjparser` to `libjexpr`.

1.5.1
-----

* All programs must check the value of `J_WANT_INSTALL`.  If it is not
  zero, they should install themselves into `J_INSTALL_ROOT`.  The reason
  for the double negative is that this allows the default action to be to
  install the programs.
* All programs and libraries must define `SEARCHDIRS` with `=` not `:=` so that
  `J_X11_INCLUDE_DIR` will work.
* Fixed ACE build system to never build with threads because some
  functions (e.g. tmpnam()) don't work the normal way on some systems
  (e.g. HP-UX 11) when threads are used.

1.5.0
-----

* Long overdue change in minor version number.
* Moved `jxuninstall` target into `include/make/default_lib_target` so
  `Make.header` for libraries no longer needs to define it.
* Added `J_STRIP_DEBUG` because Solaris strip uses `-x` instead of `-g`.

1.1.23
------

* Added `J_SHARED_LIB_LINK_OPTION` because Solaris `ld` 3.0 uses `-G` instead
  of the defacto `-shared`.  All library `Make.header` files must switch to
  using this variable in `LDFLAGS`.
* Added `J_X11_INCLUDE_DIR` because some people are cursed with
  non-standard X installations.  All `Make.header` files must include
  `${J_X11_INCLUDE_DIR}` (*not* `-I${J_X11_INCLUDE_DIR}`) at the end of
  `SEARCHDIRS`.
* Added `Make.files_template` and `Make.header_template` in `doc` directory.
  These show the minimal requirements for writing a JX program.

1.1.21
------

* Works with egcs!  Factory methods are used to call the offending code
  after the constructor has finished.  The constructor is protected to
  force you to call the factory methods instead.
* If `J_BUILD_SHARED_LIB` is defined, programs should depend on `.so`
  versions of libraries instead of `.a` versions.
* Created `include/make/default_lib_target` to hide `J_BUILD_SHARED_LIB` and
  `J_NEED_SEPARATE_O_FILES`.

1.1.20.1
--------

* Added `J_NEED_SEPARATE_O_FILES`.  If this and `J_BUILD_SHARED_LIB` are
  both defined, libraries should only build the `.so` version.

1.1.20
------

* Fixed configuration so the standard ACE distribution works again.
* If `make` is run as root, libraries and binaries are automatically
  installed in system directories.  (3rd party libraries need to add code
  to their default build target and also add a new target `jxuninstall`.)

1.1.19
------

* For each `*_VERSION` constant, created new `*_LIB_VERSION` constant.  This
  defines the library version used in file names.
* Merged `libjxparser` into `libjparser`.
* Merged `libjx3d` into `libj3d`.
