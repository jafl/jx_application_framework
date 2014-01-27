# Makefile to compile JX source distribution

# If ACE_ROOT is defined, we toss the copy included in the distribution and
# create a symbolic link to the existing directory.

ifndef ACE_ROOT
  DEFAULT_ACE_ROOT := ${shell pwd}/ACE/ACE_wrappers
endif

#
# tell user what options are available
#

.PHONY : default
default:
	@echo

  ifndef ACE_ROOT
	@echo "If you already have a compatible version of ACE on your computer, then"
	@echo "set ACE_ROOT to point to the top level ACE directory.  (This is part of"
	@echo "the standard ACE configuration.)  If you don't have ACE installed, add"
	@echo "the following to ~/.cshrc, if you use csh or tcsh:"
	@echo
	@echo "  setenv ACE_ROOT ${DEFAULT_ACE_ROOT}"
	@echo
	@echo "If you use bash, add the following to ~/.bashrc instead:"
	@echo
	@echo "  export ACE_ROOT=${DEFAULT_ACE_ROOT}"
	@echo
  endif

	@echo "If you have compiled a compatible version of Mesa on your computer, then"
	@echo "create a symbolic link ${shell pwd}/Mesa"
	@echo "which points to the directory in which you built Mesa."
	@echo

 ifeq ($(shell whoami),root)

  ifndef JX_INSTALL_ROOT
	@echo "To install the binaries in a directory other than /usr/bin, set the"
	@echo "environment variable JX_INSTALL_ROOT to the desired directory."
	@echo "If you do this, JX_INSTALL_ROOT must be on your execution path."
  endif
	@echo
  ifndef JX_LIB_ROOT
	@echo "To install the libraries in a directory other than /usr/lib, set the"
	@echo "environment variable JX_LIB_ROOT to the desired directory."
  endif

 else

  ifndef JX_INSTALL_ROOT
	@echo "To install the binaries in a directory other than ~/bin, (or ./bin, if"
	@echo "~/bin doesn't exist), set the environment variable JX_INSTALL_ROOT to"
	@echo "the desired directory.  (Use setenv if you use csh or export if you use bash.)"
	@echo
	@echo "This path (either ~/bin or JX_INSTALL_ROOT), must be on your execution path."
	@echo "(Use the \"set path\" command.)"
  endif

 endif

	@echo
	@echo "Then run one of the following or check the README file:"
	@echo
	@echo "  ${MAKE} cygwin32       for Cygwin on Windows"
	@echo "  ${MAKE} darwin_leopard for Macintosh OS X 10.5"
	@echo "  ${MAKE} darwin_snow    for Macintosh OS X 10.6"
	@echo "  ${MAKE} darwin_lion    for Macintosh OS X 10.7"
	@echo "  ${MAKE} darwin_mav     for Macintosh OS X 10.9"
	@echo "  ${MAKE} linux_intel    for Linux with g++ 3.x or above"
	@echo "  ${MAKE} linux_intel-64 for Linux with g++ 3.x or above on 64-bit systems"
	@echo
	@echo "If you are not sure which system you have, run \"uname -a\"."
	@echo

	@echo
	@echo "To clean up afterwards, you can use:"
	@echo
	@echo "  ${MAKE} tidy           removes .o files"
	@echo "  ${MAKE} clean          removes .o files and libraries"

  ifeq ($(shell whoami),root)
	@echo "  ${MAKE} uninstall      removes installed binaries and libraries"
  else
	@echo "  ${MAKE} uninstall      removes installed binaries"
	@echo
	@echo "The libraries are not installed in any system directories, so only use"
	@echo "\"clean\" if you link everything statically."
  endif

  ifeq ($(findstring CVS, $(shell ls)), CVS)
	@echo
	@echo "For CVS users:"
	@echo
	@echo "  ${MAKE} -f lib/Makefile_CVS cvsprep"
	@echo "    prepares directory structure after 'cvs get'"
	@echo
	@echo "  ${MAKE} -f lib/Makefile_CVS cvsupdate"
	@echo "    gets latest source from CVS and prepares for build"
  endif

	@echo

#
# Useful variables
#

# ${JMAKE} insures that ACE_ROOT is passed to all other Makefiles
# This can't be done earlier, because jx_constants redefines JX_INSTALL_ROOT,
# thereby cancelling the message about it.

JX_ROOT := .
include ${JX_ROOT}/include/make/jx_constants

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_ROOT}

INSTALL_CMD := cd lib; ${JMAKE} install

#
# Useful macros
#

TEST_ACE_CONFIG = \
    if { test ! -f ACE/ACE_wrappers/ace/config.h; } then \
        { \
        ln -sf

CREATE_ACE_CONFIG = \
        ACE/ACE_wrappers/ace/config.h; \
        } \
    fi

TEST_ACE_MACROS = \
    if { test ! -f ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; } then \
        { \
        ln -sf

CREATE_ACE_MACROS = \
        ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; \
        } \
    fi

#
# glibc 2.8
#

.PHONY : linux_intel
linux_intel: linux_common
	@ln -sf sys/Linux-Intel-32  include/make/jx_config
	@${INSTALL_CMD}

#
# 64-bit RedHat 5.x
#

.PHONY : linux_intel-64
linux_intel-64: linux_common
	@ln -sf sys/Linux-Intel-64  include/make/jx_config
	@${INSTALL_CMD}

#
# Linux shared code
#

.PHONY : linux_common
linux_common: prep
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-linux.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_linux.GNU ${CREATE_ACE_MACROS}

#
# Cygwin
#

.PHONY : cygwin32
cygwin32: prep
	@ln -sf sys/Linux-Intel-cygwin32 \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_CygWin.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-cygwin32.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_cygwin32.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# OS X
#

.PHONY: darwin_leopard 
darwin_leopard: prep
	@ln -sf sys/OSX_5_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-leopard.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_leopard.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_snow
darwin_snow: prep
	@ln -sf sys/OSX_6_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-snowleopard.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_snowleopard.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_lion
darwin_lion: prep
	@ln -sf sys/OSX_6_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-lion.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_lion.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_mav
darwin_mav: prep
	@ln -sf sys/OSX_9_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-mountainlion.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_mountainlion.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# shared code
#

.PHONY : prep
prep: check_install_dir fix_ace fix_mesa clean_links
	@if { test ! -e libjcore/code/jStringData.h; } then \
         { \
         cp -f libjcore/jStringData_init.h libjcore/code/jStringData.h; \
         chmod u+w libjcore/code/jStringData.h; \
         } \
     fi

# Solaris test doesn't support ! operator

.PHONY : check_install_dir
check_install_dir:
  ifdef JX_INSTALL_ROOT
	@if { test -d ${JX_INSTALL_ROOT} -a -w ${JX_INSTALL_ROOT}; } then \
         { \
         true; \
         } \
     else \
         { \
         echo; \
         echo "Please set JX_INSTALL_ROOT to a valid directory and make sure that"; \
         echo "it is on your execution path."; \
         echo; \
         false; \
         } \
     fi
  endif

#	@if { test -d ${ACE_ROOT} -a ! ACE/ACE_wrappers -ef ${ACE_ROOT}; } then

CHECK_ACE := lib/util/j_has_ace

CC := ${shell if { which gcc > /dev/null; } \
              then { echo gcc; } \
              else { echo ${CC}; } fi }

.PHONY : fix_ace
fix_ace:
  ifdef ACE_ROOT
	@if { test ! -d ACE/ACE_wrappers || \
          { ${CC} ${CHECK_ACE}.c -o ${CHECK_ACE}; \
            ${CHECK_ACE} ${ACE_ROOT} ACE/ACE_wrappers; } ; } then \
         { \
         ${RM} -r ACE/ACE_wrappers; \
         ln -sf ${ACE_ROOT} ACE/ACE_wrappers; \
         } \
     fi;
	@if { test ! '(' -f ${ACE_ROOT}/ace/Makefile -o -f ${ACE_ROOT}/ace/GNUmakefile ')'; } then \
         { \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.a; \
         ln -sf /usr/lib/libACE.a lib/libACE-${ACE_LIB_VERSION}.a; \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.so; \
         ln -sf /usr/lib/libACE.so lib/libACE-${ACE_LIB_VERSION}.so; \
         ${RM} lib/libACE.so.${ACE_VERSION}; \
         } \
     fi
  else
	@if { test ! -e ${DEFAULT_ACE_ROOT}; } then \
         if { which wget; } then \
             wget -O ACE/ACE.tgz http://newplanetsoftware.com/ftp/misc/ACE-${ACE_VERSION}.tar.gz; \
         elif { which curl; } then \
             curl -o ACE/ACE.tgz http://newplanetsoftware.com/ftp/misc/ACE-${ACE_VERSION}.tar.gz; \
         else \
             echo "Please install either curl or wget"; \
             exit 1; \
         fi; \
         cd ACE; tar -xzf ACE.tgz; touch ACE_wrappers/${ACE_VERSION}; ./patch_ace; \
     fi
	@if { test ! '(' -f ${DEFAULT_ACE_ROOT}/ace/Makefile -o -f ${DEFAULT_ACE_ROOT}/ace/GNUmakefile ')'; } then \
         { \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.a; \
         ln -sf /usr/lib/libACE.a lib/libACE-${ACE_LIB_VERSION}.a; \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.so; \
         ln -sf /usr/lib/libACE.so lib/libACE-${ACE_LIB_VERSION}.so; \
         ${RM} lib/libACE.so.${ACE_VERSION}; \
         } \
     fi
  endif

.PHONY : fix_mesa
fix_mesa:
	@if { test ! -e Mesa ; } then \
         if { which wget; } then \
             wget -O misc/Mesa.tgz http://newplanetsoftware.com/ftp/misc/MesaLib-${MESA_VERSION}.tar.gz; \
         elif { which curl; } then \
             curl -o misc/Mesa.tgz http://newplanetsoftware.com/ftp/misc/MesaLib-${MESA_VERSION}.tar.gz; \
         else \
             echo "Please install either curl or wget"; \
             exit 1; \
         fi; \
         tar -xzf misc/Mesa.tgz; mv Mesa-${MESA_VERSION} Mesa; \
     fi
	@if { test -d Mesa/. ; } then \
         { \
         ln -sf ../../src/mesa/drivers/x11/xmesa.h   Mesa/include/GL/xmesa.h; \
         ln -sf ../../src/mesa/drivers/x11/xmesa_x.h Mesa/include/GL/xmesa_x.h; \
         } \
     fi

.PHONY : clean_links
clean_links:
	@${RM} include/make/jx_config
	@${RM} include/jcore/jMissingProto.h
	@if { test ! -L ACE/ACE_wrappers; } then \
         { \
         ${RM} ACE/ACE_wrappers/ace/config.h; \
         ${RM} ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; \
         } \
     fi

#
# clean up
#

.PHONY : tidy
tidy:
	@cd lib; ${JMAKE} tidy

.PHONY : clean
clean:
	@cd lib; ${JMAKE} clean

.PHONY : uninstall
uninstall:
	@cd lib; ${JMAKE} uninstall
