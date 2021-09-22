JX_ROOT  := ..

include ${JX_ROOT}/include/make/jx_constants
include ${JX_ROOT}/include/make/jx_config

ACE_MAKE_FLAGS := exceptions=0 shared_libs=0 static_libs=1

ifeq (${J_OPTIMIZE_LEVEL},-O2)
  ACE_DEBUG := debug=0
else
  ACE_DEBUG := debug=1
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
jxinstall:
	@if [[ -x ACE_wrappers/bin/mwc.pl && \
           ! -f ${ACE_ROOT}/ace/Makefile && \
           ! -f ${ACE_ROOT}/ace/GNUmakefile ]]; \
     then \
         cd ACE_wrappers; ./bin/mwc.pl -type gnuace ACE.mwc; \
     fi
	@${BEGIN_ACE_MAKE} ${ACE_DEBUG} ${ACE_MAKE_FLAGS} ${END_ACE_MAKE}
  ifeq ($(shell whoami),root)
	cp ACE_wrappers/ace/libACE.a ${JX_LIB_ROOT}/libACE.a; \
  endif

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
