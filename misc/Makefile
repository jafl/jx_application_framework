JX_ROOT  := ..
MAKE_DIR := ${shell pwd}

include ${JX_ROOT}/include/make/jx_constants
include ${JX_ROOT}/include/make/jx_config

#
# JX source distribution (DISTR_TAR_FILE)
#

SRC_DIRS := ${addprefix ${JX}/misc/, \
              ${wildcard *} }

.PHONY : jxsource
jxsource: clean
	@cd ${J_DISTR_TAR_DIR}; \
     tar -rf ${DISTR_TAR_FILE} ${SRC_DIRS} ${JX}/lib/libXpm.*

#
# remove object files
#

.PHONY : tidy
tidy:: clean

.PHONY : clean
clean::
	@${foreach dir, \
               ${filter-out ./CVS/., ${wildcard ./*/.}}, \
               cd ${dir}; ${MAKE} clean > /dev/null; cd ${MAKE_DIR};}
	@rm -f pcre/Makefile pcre/libtool
