JX_ROOT := ..

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

ifeq (${J_OPTIMIZE_LEVEL},-O2)
  ACE_DEBUG := debug=0
else
  ACE_DEBUG := debug=1
endif

#
# Useful macros
#

BEGIN_ACE_CMD = \
    if [[ -f ACE_wrappers/ace/Makefile || -f ACE_wrappers/ace/GNUmakefile ]]; then

END_ACE_CMD = \
    fi

BEGIN_ACE_MAKE = \
    ${MAKE} ACE_ROOT=$$(cd ..; pwd) exceptions=0 shared_libs=0 static_libs=1

#
# ACE library
#

.PHONY : default
default:
	@if [[ -x ACE_wrappers/bin/mwc.pl && \
           ! -f ACE_wrappers/ace/Makefile && \
           ! -f ACE_wrappers/ace/GNUmakefile ]]; \
     then \
         cd ACE_wrappers; \
         export ACE_ROOT=$$(pwd); \
         ./bin/mwc.pl -type gnuace ACE.mwc; \
     fi
	@${BEGIN_ACE_CMD} \
       cd ACE_wrappers/ace; \
       ${BEGIN_ACE_MAKE} ${ACE_DEBUG}; \
     ${END_ACE_CMD}

.PHONY : install
install:
	@${BEGIN_ACE_CMD} \
       echo Please authorize sudo access for installing ACE...; \
       ${SUDO} echo sudo access authorized...; \
       cd ACE_wrappers/ace; \
       ${SUDO} ${BEGIN_ACE_MAKE} INSTALL_PREFIX=/usr/local install; \
     ${END_ACE_CMD}

.PHONY : uninstall
uninstall:
	@${BEGIN_ACE_CMD} \
       ${SUDO} rm -rf /usr/local/include/ace /usr/local/lib/libACE*; \
     ${END_ACE_CMD}

#
# remove object files
#

.PHONY : tidy
tidy::
	@if [[ -d ACE_wrappers ]]; \
     then cd ACE_wrappers/ace; ${BEGIN_ACE_MAKE} clean; fi
	@cd test; ${MAKE} tidy

.PHONY : clean
clean::
	@if [[ -d ACE_wrapper ]]; \
     then cd ACE_wrappers/ace; ${BEGIN_ACE_MAKE} realclean; fi
	@cd test; ${MAKE} clean
