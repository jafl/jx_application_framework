JX_ROOT  := ..
MAKE_DIR := ${shell pwd}

include ${JX_ROOT}/include/make/jx_constants
include ${JX_ROOT}/include/make/jx_config

#
# remove object files
#

.PHONY : tidy
tidy:: clean

.PHONY : clean
clean::
	@${foreach dir, \
               ${wildcard ./*/.}, \
               cd ${dir}; ${MAKE} clean > /dev/null; cd ${MAKE_DIR};}
	@rm -f pcre-8.10/Makefile pcre-8.10/libtool gd/*.o
	@cd ctags; ${MAKE} distclean > /dev/null
