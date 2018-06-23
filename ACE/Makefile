JX_ROOT  := ..

include ${JX_ROOT}/include/make/jx_constants
include ${JX_ROOT}/include/make/jx_config

ACE_MAKE_FLAGS := exceptions=0

ifdef J_COMPILE_DEBUG
  ACE_DEBUG := debug=1
else
  ACE_DEBUG := debug=0
endif

#
# Useful macros
#

BEGIN_ACE_MAKE = \
    if [[ -f ${ACE_ROOT}/ace/Makefile || -f ${ACE_ROOT}/ace/GNUmakefile ]]; then \
        cd ${ACE_ROOT}/ace; ${MAKE}

END_ACE_MAKE = \
    ; \
    fi

#
# ACE library
#

.PHONY : jxinstall
ifdef J_BUILD_SHARED_LIB
  jxinstall: prep shared
else
  jxinstall: prep static
endif
  ifeq ($(shell whoami),root)
	@if [[ -f ACE_wrappers/ace/libACE.a ]]; then \
         cp ACE_wrappers/ace/libACE.a ${JX_LIB_ROOT}/libACE.a; \
     fi
	@if [[ -f ACE_wrappers/ace/libACE.so ]]; then \
         cp ACE_wrappers/ace/libACE.so ${JX_LIB_ROOT}/libACE.so; \
     fi
  endif

.PHONY : prep
prep:
	@if [[ -x ACE_wrappers/bin/mwc.pl && \
           ! -f ${ACE_ROOT}/ace/Makefile && \
           ! -f ${ACE_ROOT}/ace/GNUmakefile ]]; \
     then \
         cd ACE_wrappers; ./bin/mwc.pl -type gnuace ACE.mwc; \
     fi

.PHONY : shared
shared:
	@${BEGIN_ACE_MAKE} ${ACE_MAKE_FLAGS} debug=0 shared_libs=1 static_libs=0 ${END_ACE_MAKE}

.PHONY : static
static:
	@${BEGIN_ACE_MAKE} ${ACE_DEBUG} ${ACE_MAKE_FLAGS} shared_libs=0 static_libs=1 ${END_ACE_MAKE}

#
# remove object files
#

.PHONY : tidy
tidy::
	@if { ! test -L ACE_wrappers; } \
     then { ${BEGIN_ACE_MAKE} clean shared_libs=1 static_libs=1 ${END_ACE_MAKE}; } fi
	@cd test; ${MAKE} tidy

.PHONY : clean
clean::
	@if { ! test -L ACE_wrappers; } \
     then { ${BEGIN_ACE_MAKE} realclean shared_libs=1 static_libs=1 ${END_ACE_MAKE}; } fi
	@cd test; ${MAKE} clean

.PHONY : jxuninstall
jxuninstall:
  ifeq ($(shell whoami),root)
	@if { ! test -L ACE_wrappers; } \
     then { ${RM} ${JX_LIB_ROOT}/libACE.*; } fi
  endif
