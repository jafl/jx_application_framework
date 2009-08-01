# Makefile for JX distribution

JX_ROOT := ..

JX_CORE_LIBS := ../libjcore/. \
                ../libjx/. \
                ../libjtree/. \
                ../libjtoolbar/.

DISTR_PROGS := ../programs/makemake/. \
               ../programs/compile_jstrings/. \
               ../programs/jxlayout/. \
               ../programs/jx_project_wizard/. \
               ../programs/ssh_askpass/.

include ${JX_ROOT}/include/make/jx_constants
include ${JX_ROOT}/include/make/jx_config

# useful macros

BEGIN_DIR = if test -d ${dir}; then ( cd ${dir}
END_DIR   = ) fi

#
# build all Makefiles
#

.PHONY : Makefiles
Makefiles:
	@if { test -d ${JX_ROOT}/programs/code_crusader \
               -a -d ${JX_ROOT}/programs/code_medic; } then \
         { \
         cd ${JX_ROOT}/programs/code_medic; \
         ${MAKE} -f Make.header update_jcc; \
         } \
     fi
	@${foreach dir, \
          ${filter-out %/CVS/., \
            ${wildcard ../lib?*/. ./util/*/. ../programs/*/.}} \
          ../ACE/test/ ../tutorial/, \
       ${BEGIN_DIR}; makemake; ${MAKE} Makefiles; ${END_DIR};}

#
# install binaries
#

.PHONY : install
install: install1 install2

.PHONY: install1
install1:
ifeq ($(shell which makemake),)
	@cd ../ACE; ${MAKE} jxinstall
	@cd ../programs/makemake; ${MAKE} -f Makefile.port jxinstall
	@${MAKE} Makefiles
endif

.PHONY: install2
install2:
	@cd ../libjcore; ${MAKE} COMPILE_STRINGS=0 default
	@cd ../programs/compile_jstrings; ${MAKE} jxinstall
	@${foreach dir, \
          ${wildcard ../lib?*/.}, \
       ${BEGIN_DIR}; ${MAKE} default; ${END_DIR};}
	@${foreach dir, \
          ${filter-out %/CVS/., ${wildcard ./util/*/. ../programs/*/.}}, \
       ${BEGIN_DIR}; ${MAKE} jxinstall; ${END_DIR};}

#
# build all libraries
#

.PHONY : alllib
alllib:
	@${foreach dir, ${wildcard ../lib?*/.}, \
       ${BEGIN_DIR}; makemake; ${MAKE} default; ${END_DIR};}

#
# build all test suites
#

.PHONY : alltest
alltest:
	@${foreach dir, \
          ${wildcard ../lib?*/test/.} \
          ../ACE/test/ ../tutorial/, \
       ${BEGIN_DIR}; makemake; ${MAKE} all; ${END_DIR};}

#
# build all for ~/bin
#

PERSONAL_PROGS := code_crusader code_medic code_mill systemg leibnitz arrow

.PHONY : allpersonal
allpersonal: Makefiles
	@${foreach dir, \
          ${addprefix ../programs/, ${PERSONAL_PROGS}}, \
       ${BEGIN_DIR}; ${MAKE} personal; ${END_DIR};}

#
# source distribution
#

SRC_TAR_FILE := JX_source-${JX_VERSION}.tar

.PHONY : jxsource
jxsource:
	@cd ${J_DISTR_TAR_DIR}; \
     tar -cf ${SRC_TAR_FILE} ${JX}/README ${JX}/FAQ ${JX}/CHANGES \
                             ${JX}/LICENSE ${JX}/COPYING \
                             ${JX}/INSTALL ${JX}/Makefile ${JX}/*.dsw ${JX}/*.bat

	@${foreach dir, \
          ${JX_CORE_LIBS} ../ACE/ ../tutorial/ ../misc/ ${DISTR_PROGS} \
          ${filter-out %/CVS/., ${wildcard ./util/*/.}}, \
       ${BEGIN_DIR}; \
       ${MAKE} DISTR_TAR_FILE=${SRC_TAR_FILE} jxsource; \
       ${END_DIR}; }

	@cd ${J_DISTR_TAR_DIR}; \
     tar -rf ${SRC_TAR_FILE} ${filter-out %~, \
                               ${shell cd ${J_DISTR_TAR_DIR}; echo \
             ${JX}/doc/ \
             ${JX}/include/missing_proto/* \
             ${JX}/include/make/jx_constants \
             ${JX}/include/make/nps_constants \
             ${JX}/include/make/jx_config_common \
             ${JX}/include/make/jx_targets \
             ${JX}/include/make/default_lib_target \
             ${JX}/include/make/sys/* \
             ${addprefix ${JX}/lib/, \
               Makefile* README.* \
               util/j_has_ace.c \
               ${filter-out util/encode_site_name, \
                            ${shell grep -l /usr/bin/perl util/*}} } }}; \
     gzip ${SRC_TAR_FILE}

.PHONY : alllibsource
alllibsource: Makefiles jxsource
	@${foreach dir, \
          ${filter-out ${JX_CORE_LIBS} ${DISTR_PROGS}, \
            ${wildcard ../lib?*/.}}, \
       ${BEGIN_DIR}; ${MAKE} source; ${END_DIR};}

.PHONY : allsource
allsource: Makefiles jxsource
	@${foreach dir, \
          ${filter-out ${JX_CORE_LIBS} ${DISTR_PROGS} %/CVS/., \
            ${wildcard ../lib?*/. ../programs/*/.}}, \
       ${BEGIN_DIR}; ${MAKE} source; ${END_DIR};}

#
# Release
#
# leibnitz is last so it can fail silently
#

RELEASE_PROGS := ${addprefix ../programs/, \
                   code_crusader code_medic code_mill \
                   systemg glove \
                   ssh_askpass leibnitz}

.PHONY : all_release
all_release:
	@if ! test -e ../ACE/ACE_wrappers/${ACE_VERSION}; then echo 'Wrong ACE version!'; false; fi
	@${foreach dir, ${RELEASE_PROGS}, \
       ${BEGIN_DIR}; ${MAKE} release_static; ${END_DIR}; }

.PHONY : all_rpm
all_rpm:
	@${foreach dir, ${RELEASE_PROGS}, \
       ${BEGIN_DIR}; ${MAKE} release_rpm; ${END_DIR}; }

#
# Demo
#

DEMO_PROGS := ${addprefix ../programs/, \
                code_crusader}

.PHONY : all_demo
all_demo:
	@${foreach dir, ${DEMO_PROGS}, \
       ${BEGIN_DIR}; ${MAKE} demo; ${END_DIR}; }

.PHONY : all_demo_rpm
all_demo_rpm:
	@${foreach dir, ${DEMO_PROGS}, \
       ${BEGIN_DIR}; ${MAKE} demo_rpm; ${END_DIR}; }

DEMO_LIMIT_PROGS := ${addprefix ../programs/, \
                      code_crusader code_medic}

.PHONY : all_demo_limit
all_demo_limit:
	@${foreach dir, ${DEMO_LIMIT_PROGS}, \
       ${BEGIN_DIR}; ${MAKE} demo_limit; ${END_DIR}; }
#	@${foreach dir, ../programs/glove, \
#       ${BEGIN_DIR}; ${MAKE} beta_limit; ${END_DIR}; }

.PHONY : all_demo_limit_rpm
all_demo_limit_rpm:
	@${foreach dir, ${DEMO_LIMIT_PROGS}, \
       ${BEGIN_DIR}; ${MAKE} demo_limit_rpm; ${END_DIR}; }

#
# clean up
#

.PHONY : tidy
tidy:
	@${foreach dir, \
          ${filter-out %/CVS/., \
            ${wildcard ../lib?*/. ../programs/*/. ./util/*/.}} \
          ../ACE/ ../misc/ ../tutorial/, \
       ${BEGIN_DIR}; ${MAKE} tidy; ${END_DIR};}

.PHONY : clean
clean:
	@${foreach dir, \
          ${filter-out %/CVS/., \
            ${wildcard ../lib?*/. ../programs/*/. ./util/*/.}} \
          ../ACE/ ../misc/ ../tutorial/, \
       ${BEGIN_DIR}; ${MAKE} clean; ${END_DIR};}

.PHONY : uninstall
uninstall:
	@${foreach dir, \
          ${filter-out %/CVS/., \
            ${wildcard ../lib?*/. ../programs/*/. ./util/*/.}} \
          ../ACE/, \
       ${BEGIN_DIR}; ${MAKE} jxuninstall; ${END_DIR};}
