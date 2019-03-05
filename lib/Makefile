# Makefile for JX distribution

JX_ROOT := ..

JX_CORE_LIBS := ../libjcore/. \
                ../libjx/. \
                ../libjtree/. \
                ../libjtoolbar/.

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
	@if [[ -d ${JX_ROOT}/programs/code_crusader && \
            -d ${JX_ROOT}/programs/code_medic ]]; \
      then \
         cd ${JX_ROOT}/programs/code_medic; \
         ${MAKE} -f Make.header update_jcc; \
      fi
	@${foreach dir, \
          ${wildcard ../lib?*/. ../tools/*/. ../programs/*/.} \
          ../ACE/test/ ../tutorial/, \
       ${BEGIN_DIR}; makemake; ${MAKE} Makefiles; ${END_DIR};}
	@cd ../misc; ${MAKE} Makefiles

#
# build all layouts
#

.PHONY : layouts
layouts:
	@${foreach dir, \
          ${wildcard ../lib?*/. ../tools/*/. ../programs/*/.}, \
       ${BEGIN_DIR}; \
           if compgen -G "*.fd" > /dev/null; then \
               jxlayout --require-obj-names *.fd; \
           fi; \
       ${END_DIR};}

#
# install binaries
#

.PHONY : install
install: install_pre install_libs install_apps

.PHONY: install_pre
install_pre:
	@if { ! test -e ./libACE-*.so -o -e ./libACE-*.a; } then \
         cd ../ACE; ${MAKE} jxinstall; \
     fi
	@if { ! test -x ../tools/makemake/makemake; } then \
         cd ../tools/makemake; \
         ${MAKE} -f Makefile.port jxinstall; \
     fi
	@if { ! test -f ../libjcore/Makefile; } then \
         ${MAKE} Makefiles; \
     fi

.PHONY: install_libs
install_libs:
	@cd ../libjcore; ${MAKE} COMPILE_STRINGS=0 default
	@cd ../tools/compile_jstrings; ${MAKE} jxinstall
	@cd ../libjcore; ${MAKE} jx.test.skip=true
	@for dir in libjx libjfs libjexpr libj2dplot; do \
       if ! ( cd ../$$dir; ${MAKE} default; ); then exit 1; fi \
     done;

.PHONY: install_apps
install_apps:
	@${foreach dir, \
          ${wildcard ../tools/*/. ../programs/*/.}, \
       ${BEGIN_DIR}; ${MAKE} jxinstall; ${END_DIR};}

#
# build all libraries
#

.PHONY : lib
lib:
	@${foreach dir, ${wildcard ../lib?*/.}, \
       ${BEGIN_DIR}; makemake; ${MAKE} default; ${END_DIR};}

#
# run all test suites
#

.PHONY : test
test: install_pre install_libs
ifeq (${J_RUN_GCOV},1)
	@find .. \( -name '*.gcno' -or -name '*.gcda' \) -and -not -path '*/libjcore/code/*' -exec rm -f '{}' +
	@cd ../libjcore; p=`pwd`; \
     for f in `find . -name '*.gcno'`; do \
         root=$$p/$${f%/*}; \
         gcov -lp --object-directory $$root $$p/$${f%.*}.o > /dev/null; \
         mv *.gcov $$root; \
     done
endif

#
# build all for ~/bin
#

PERSONAL_TOOLS := \
    makemake compile_jstrings jxlayout webgif \
    jx_project_wizard jx_memory_debugger

PERSONAL_PROGS := \
    code_crusader code_medic code_mill \
    drakon leibnitz systemg svn_client ssh_askpass

.PHONY : personal
personal:
	@${foreach dir, \
          ${addprefix ../tools/,    ${PERSONAL_TOOLS}} \
          ${addprefix ../programs/, ${PERSONAL_PROGS}}, \
       ${BEGIN_DIR}; ${MAKE} personal; ${END_DIR};}

#
# Release
#

RELEASE_PROGS := ${addprefix ../programs/, \
                   code_crusader code_medic code_mill \
                   systemg ssh_askpass svn_client \
                   drakon leibnitz glove }

.PHONY : release
release:
	@${foreach dir, ${RELEASE_PROGS}, \
       ${BEGIN_DIR}; ${MAKE} release; ${END_DIR}; }

#
# clean up
#

.PHONY : tidy
tidy:
	@${foreach dir, \
          ${wildcard ../lib?*/. ../tools/*/. ../programs/*/.} \
          ../ACE/ ../misc/ ../tutorial/, \
       ${BEGIN_DIR}; ${MAKE} tidy; ${END_DIR};}

.PHONY : clean
clean:
	@${foreach dir, \
          ${wildcard ../lib?*/. ../tools/*/. ../programs/*/.} \
          ../ACE/ ../misc/ ../tutorial/, \
       ${BEGIN_DIR}; ${MAKE} clean; ${END_DIR};}

.PHONY : uninstall
uninstall:
	@${foreach dir, \
          ${wildcard ../lib?*/. ../tools/*/. ../programs/*/.} \
          ../ACE/, \
       ${BEGIN_DIR}; ${MAKE} jxuninstall; ${END_DIR};}
