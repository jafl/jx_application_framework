JX_ROOT  := ..

include ${JX_ROOT}/include/make/jx_constants
include ${JX_ROOT}/include/make/jx_config

ACE_MAKE_FLAGS := 

ifeq (${J_USE_THREADS},yes)
  ACE_MAKE_FLAGS += threads=1
else
  ACE_MAKE_FLAGS += threads=0
endif

ifdef J_COMPILE_DEBUG
  ACE_DEBUG := debug=1
else
  ACE_DEBUG := debug=0
endif

#
# Useful macros
#

BEGIN_ACE_MAKE = \
    if { test -f ${ACE_ROOT}/ace/Makefile -o -f ${ACE_ROOT}/ace/GNUmakefile; } then \
        { \
        cd ${ACE_ROOT}/ace; ${MAKE}

END_ACE_MAKE = \
    ; } \
    fi

#
# ACE library
#

.PHONY : jxinstall
ifdef J_FORCE_ACE_STATIC
  jxinstall: static
	@${RM} -r ACE_wrappers/ace/libACE.so ACE_wrappers/ace/.shobj/
else
ifdef J_BUILD_SHARED_LIB
  jxinstall: shared
else
  jxinstall: static
endif
endif
  ifeq ($(shell whoami),root)
	@if { test -f ACE_wrappers/ace/libACE.a; } then \
         { \
         cp ACE_wrappers/ace/libACE.a ${JX_LIB_ROOT}/libACE-${ACE_LIB_VERSION}.a; \
         } \
     fi
	@if { test -f ACE_wrappers/ace/libACE.so; } then \
         { \
         cp ACE_wrappers/ace/libACE.so ${JX_LIB_ROOT}/libACE-${ACE_LIB_VERSION}.so; \
         } \
     fi
  endif

.PHONY : shared
shared:
	@${BEGIN_ACE_MAKE} ${ACE_MAKE_FLAGS} debug=0 shared_libs=1 static_libs=0 ${END_ACE_MAKE}

.PHONY : static
static:
	@${BEGIN_ACE_MAKE} ${ACE_DEBUG} ${ACE_MAKE_FLAGS} shared_libs=0 static_libs=1 ${END_ACE_MAKE}

#
# JX source distribution (DISTR_TAR_FILE)
#

SRC_FILE_DIR := ${JX}/ACE

EXTRA_ACE_FILES := ${ACE_VERSION} VERSION ACE-INSTALL* docs/ \
                   AUTHORS FAQ README COPYING PROBLEM-REPORT-FORM \
                   bin/ace_components etc/ 

.PHONY : jxsource
jxsource:
	@cd ${J_DISTR_TAR_DIR}; \
     tar -rf ${DISTR_TAR_FILE} \
             ${SRC_FILE_DIR}/README \
             ${SRC_FILE_DIR}/Makefile \
             ${JX}/lib/libACE-${ACE_LIB_VERSION}.* \
             ${JX}/lib/libACE.so.${ACE_VERSION} \
             ${addprefix ${SRC_FILE_DIR}/ACE_wrappers/, ${EXTRA_ACE_FILES} }
	@cd ${J_DISTR_TAR_DIR}; \
     tar -rf ${DISTR_TAR_FILE} ${filter-out %.a %.so %~ %/ace/config.h, \
                                 ${shell cd ${J_DISTR_TAR_DIR}; echo \
             ${SRC_FILE_DIR}/ACE_wrappers/ace/* \
             ${SRC_FILE_DIR}/ACE_wrappers/include/makeinclude/* }};
	@cd test; ${MAKE} jxsource

#
# remove object files
#

ACE_TIDY      := ${BEGIN_ACE_MAKE} clean shared_libs=1 static_libs=1 ${END_ACE_MAKE}
ACE_CLEAN     := ${BEGIN_ACE_MAKE} realclean shared_libs=1 static_libs=1 ${END_ACE_MAKE}
ACE_UNINSTALL := ${RM} ${JX_LIB_ROOT}/libACE-${ACE_LIB_VERSION}.*

.PHONY : tidy
tidy::
	@if { ! test -L ACE_wrappers; } \
     then { ${ACE_TIDY}; } fi
	@cd test; ${MAKE} tidy

.PHONY : clean
clean::
	@if { ! test -L ACE_wrappers; } \
     then { ${ACE_CLEAN}; } fi
	@cd test; ${MAKE} clean

.PHONY : jxuninstall
jxuninstall:
  ifeq ($(shell whoami),root)
	@if { ! test -L ACE_wrappers; } \
     then { ${ACE_UNINSTALL}; } fi
  endif
