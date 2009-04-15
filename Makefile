# Makefile to compile JX source distribution

# If ACE_ROOT is defined, we toss the copy included in the distribution and
# create a symbolic link to the existing directory.

ifndef ACE_ROOT
  DEFAULT_ACE_ROOT := ${shell cd ACE/ACE_wrappers; pwd}
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
#	@echo "  ${MAKE} aix4_x-g++     for AIX 4.x with g++"
#	@echo "  ${MAKE} corel1_x       for CorelLinux 1.x"
	@echo "  ${MAKE} cygwin32       for Cygwin on Windows"
	@echo "  ${MAKE} darwin_tiger   for Macintosh OS X 10.4"
	@echo "  ${MAKE} darwin_leopard for Macintosh OS X 10.5"
#	@echo "  ${MAKE} debian1_x      for Debian 1.x GNU/Linux"
#	@echo "  ${MAKE} debian2_x      for Debian 2.x GNU/Linux"
	@echo "  ${MAKE} debian3_1      for Debian 3.1 GNU/Linux (Sarge)"
#	@echo "  ${MAKE} freebsd2_x     for FreeBSD 2.x"
#	@echo "  ${MAKE} freebsd3_x     for FreeBSD 3.x"
#	@echo "  ${MAKE} hpux10-g++     for HP-UX 10 with g++"
#	@echo "  ${MAKE} irix5_3-g++    for IRIX 5.3 with g++"
#	@echo "  ${MAKE} irix6_x-g++    for IRIX 6.x with g++"
#	@echo "  ${MAKE} linuxppc4      for LinuxPPC r4"
#	@echo "  ${MAKE} linuxppc5      for LinuxPPC r5"
#	@echo "  ${MAKE} mklinux        for MkLinux"
#	@echo "  ${MAKE} openbsd2_x     for OpenBSD 2.x"
#	@echo "  ${MAKE} openlinux2_2   for OpenLinux 2.2"
#	@echo "  ${MAKE} osf1_4_x-g++   for Digital Unix (osf1) 4.x with g++"
	@echo "  ${MAKE} linux_intel    for Linux with g++ 3.x or above"
	@echo "  ${MAKE} linux_intel-64 for Linux with g++ 3.x or above on 64-bit systems"
#	@echo "  ${MAKE} redhat5_x-kcc  for RedHat 5.x & 6.x Linux with KAI C++"
#	@echo "  ${MAKE} slackware      for Slackware Linux"
	@echo "  ${MAKE} sunos4-g++     for SunOS 4.x and g++"
	@echo "  ${MAKE} sunos5_4-g++   for SunOS 5.4 (Solaris 2.4) with g++"
	@echo "  ${MAKE} sunos5_5-g++   for SunOS 5.5 (Solaris 2.5) with g++"
	@echo "  ${MAKE} sunos5_6-g++   for SunOS 5.6 (Solaris 2.6) with g++"
	@echo "  ${MAKE} sunos5_7-g++   for SunOS 5.7 (Solaris 7)   with g++"
	@echo "  ${MAKE} sunos5_8-g++   for SunOS 5.8 (Solaris 8)   with g++"
	@echo "  ${MAKE} sunos5_9-g++   for SunOS 5.9 (Solaris 9)   with g++"
#	@echo "  ${MAKE} suse5_x        for SuSE 5.x Linux"
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
        ln -s

CREATE_ACE_CONFIG = \
        ACE/ACE_wrappers/ace/config.h; \
        } \
    fi

TEST_ACE_MACROS = \
    if { test ! -f ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; } then \
        { \
        ln -s

CREATE_ACE_MACROS = \
        ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; \
        } \
    fi

#
# RedHat 8.x, 9.x, Fedora (gcc 3.x)
#

.PHONY : linux_intel
linux_intel: linux_common
	@ln -s sys/Linux-Intel-gcc3  include/make/jx_config
	@${INSTALL_CMD}

#
# 64-bit RedHat 5.x
#

.PHONY : linux_intel-64
linux_intel-64: linux_common
	@ln -s sys/Linux-Alpha-gcc3  include/make/jx_config
	@${INSTALL_CMD}

#
# RedHat 5.x with KAI C++
#

.PHONY : redhat5_x-kcc
redhat5_x-kcc: prep
	@ln -s sys/Linux-Intel-RH5.x-kcc \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-linux-kcc.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_linux_kcc.GNU ${CREATE_ACE_MACROS}

	@${INSTALL_CMD}

#
# Debian
#

.PHONY : debian1_x
debian1_x: redhat4.2

.PHONY : debian2_x
debian2_x: redhat5.x

.PHONY : debian3_1
debian3_1: linux_common
	@ln -s sys/Linux-Intel-Debian3.1  include/make/jx_config
	@${INSTALL_CMD}

#
# SuSE
#

.PHONY : suse5_x
suse5_x: linux_common
	@ln -s sys/Linux-Intel-SuSE5.x  include/make/jx_config
	@${INSTALL_CMD}

#
# Slackware
#

.PHONY : slackware
slackware: suse5_x

#
# OpenLinux
#

.PHONY : openlinux2_2
openlinux2_2: linux_common
	@ln -s sys/Linux-Intel-OpenLinux2.2  include/make/jx_config
	@${INSTALL_CMD}

#
# Corel Linux 1.x
#

.PHONY : corel1_x
corel1_x: linux_common
	@ln -s sys/Linux-Intel-Corel1.0  include/make/jx_config
	@${INSTALL_CMD}

#
# LinuxPPC r4
#

.PHONY : linuxppc4
linuxppc4:
	@ln -s sys/LinuxPPC_4  include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-linuxppc4.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_linux.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# LinuxPPC r5
#

.PHONY : linuxppc5
linuxppc5: linux_common
	@ln -s sys/LinuxPPC_5  include/make/jx_config
	@${INSTALL_CMD}

#
# MkLinux
#

.PHONY : mklinux
mklinux: linux_common
	@ln -s sys/MkLinux_g++  include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-mklinux.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_linux.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# Linux shared code
#

.PHONY : linux_common
linux_common: prep
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-linux.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_linux.GNU ${CREATE_ACE_MACROS}

#
# Cygwin
#

.PHONY : cygwin32
cygwin32: prep
	@ln -s sys/Linux-Intel-cygwin32 \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_CygWin.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-cygwin32.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_cygwin32.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# FreeBSD
#

.PHONY : freebsd2_x
freebsd2_x: prep
	@ln -s sys/FreeBSD-2.x_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-freebsd.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_freebsd.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY : freebsd3_x
freebsd3_x: prep
	@ln -s sys/FreeBSD-3.x_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-freebsd-pthread.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_freebsd_pthread.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# OpenBSD
#

.PHONY: openbsd2_x
openbsd2_x: prep
	@ln -s sys/OpenBSD_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-openbsd.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_openbsd.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# OS X
#

.PHONY: darwin_tiger 
darwin_tiger: prep
	@ln -s sys/OSX_4_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-tiger.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_tiger.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_leopard 
darwin_leopard: prep
	@ln -s sys/OSX_5_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-leopard.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_leopard.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# SunOS 4.x
#

.PHONY : sunos4-g++
sunos4-g++: prep
	@ln -s sys/SunOS-4_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_SunOS4.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-sunos4-g++.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_sunos4_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# SunOS 5.4 (Solaris 2.4)
#

.PHONY : sunos5_4-g++
sunos5_4-g++: prep
	@ln -s sys/SunOS-5.4_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_SunOS5.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-sunos5.4-g++.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_sunos5_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# SunOS 5.5 (Solaris 2.5)
#

.PHONY : sunos5_5-g++
sunos5_5-g++: prep
	@ln -s sys/SunOS-5.5_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_SunOS5.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-sunos5.5.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_sunos5_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# SunOS 5.6 (Solaris 2.6)
#

.PHONY : sunos5_6-g++
sunos5_6-g++: prep
	@ln -s sys/SunOS-5.6_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_SunOS5.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-sunos5.6.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_sunos5_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# SunOS 5.7 (Solaris 7)
#

.PHONY : sunos5_7-g++
sunos5_7-g++: prep
	@ln -s sys/SunOS-5.7_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_SunOS5.7.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-sunos5.7.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_sunos5_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# SunOS 5.8 (Solaris 8)
#

.PHONY :	sunos5_8-g++
sunos5_8-g++: prep
	@ln -s sys/SunOS-5.7_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_SunOS5.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-sunos5.8.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_sunos5_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# SunOS 5.9 (Solaris 9)
#

.PHONY :	sunos5_9-g++
sunos5_9-g++: prep
	@ln -s sys/SunOS-5.9_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_SunOS5.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-sunos5.9.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_sunos5_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# HP-UX 10
#

.PHONY : hpux10-g++
hpux10-g++: prep
	@ln -s sys/HP-UX10_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_HPUX10.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-hpux-10.x-g++.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_hpux_gcc.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# IRIX 5.3
#

.PHONY : irix5_3-g++
irix5_3-g++: prep
	@ln -s sys/IRIX-5.x_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_IRIX5.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-irix5.3-g++.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_irix5.3_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# IRIX 6.x
#

.PHONY : irix6_x-g++
irix6_x-g++: prep
	@ln -s sys/IRIX-6.x_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_IRIX6.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-irix6.x-g++.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_irix6.x_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# Digital UNIX (OSF1) 4.x
#

.PHONY : osf1_4_x-g++
osf1_4_x-g++: prep
	@ln -s sys/OSF1-4.x_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_empty.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-osf1-4.0.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_osf1_4.0_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# AIX 4.x
#

.PHONY : aix4_x-g++
aix4_x-g++: prep
	@ln -s sys/AIX-4.x_g++ \
           include/make/jx_config
	@ln -s ../../include/missing_proto/jMissingProto_AIX4.x.h \
           include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-aix-4.x.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_aix4_g++.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# shared code
#

.PHONY : prep
prep: check_install_dir fix_ace clean_links
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
         ln -s ${ACE_ROOT} ACE/ACE_wrappers; \
         } \
     fi; \
     if { test ! '(' -f ${ACE_ROOT}/ace/Makefile -o -f ${ACE_ROOT}/ace/GNUmakefile ')'; } then \
         { \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.a; \
         ln -s /usr/lib/libACE.a lib/libACE-${ACE_LIB_VERSION}.a; \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.so; \
         ln -s /usr/lib/libACE.so lib/libACE-${ACE_LIB_VERSION}.so; \
         ${RM} lib/libACE.so.${ACE_VERSION}; \
         } \
     fi
  endif

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
