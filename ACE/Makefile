JX_ROOT := ..

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

ifndef ACE_ROOT
  export ACE_ROOT = ${shell cd ACE_wrappers; pwd}
endif

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

.PHONY : default
default:
	@if [[ -x ACE_wrappers/bin/mwc.pl && \
           ! -f ${ACE_ROOT}/ace/Makefile && \
           ! -f ${ACE_ROOT}/ace/GNUmakefile ]]; \
     then \
         cd ACE_wrappers; ./bin/mwc.pl -type gnuace ACE.mwc; \
     fi
	@${BEGIN_ACE_MAKE} ${ACE_DEBUG} ${ACE_MAKE_FLAGS} ${END_ACE_MAKE}

.PHONY : install
install: default
	@echo Please authorize sudo access for building reflex...; \
	@${SUDO} echo sudo access authorized...; \
	@${SUDO} ${BEGIN_ACE_MAKE} ${ACE_MAKE_FLAGS} INSTALL_PREFIX=/usr/local install ${END_ACE_MAKE}

.PHONY : uninstall
uninstall:
	@${SUDO} rm -rf /usr/local/include/ace /usr/local/lib/libACE*

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
