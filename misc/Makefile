JX_ROOT  := ..

include ${JX_ROOT}/include/make/jx_constants
include ${JX_ROOT}/include/make/jx_config

.PHONY : Makefiles
Makefiles:
	@cd autodoc; makemake

#
# remove object files
#

.PHONY : tidy
tidy:: clean

.PHONY : clean
clean::
	@${foreach dir, \
               autodoc ctags, \
               pushd ${dir}; ${MAKE} clean > /dev/null; popd;}
