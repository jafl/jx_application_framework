JX_ROOT := ..

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

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
  ifeq ($(shell whoami),root)
	@${BEGIN_ACE_MAKE} install ${END_ACE_MAKE}
#	@if { ! test -L ACE_wrappers; } \
#     then { cp ACE_wrappers/ace/libACE.a ${JX_INSTALL_ROOT}/lib; } fi
  endif

.PHONY : uninstall
uninstall:
  ifeq ($(shell whoami),root)
	@${BEGIN_ACE_MAKE} uninstall ${END_ACE_MAKE}
#	@if { ! test -L ACE_wrappers; } \
#     then { ${RM} ${JX_INSTALL_ROOT}/lib/libACE.a; } fi
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
