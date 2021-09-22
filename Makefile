JX_ROOT := .
include ${JX_ROOT}/include/make/jx_config

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_ROOT}

.PHONY : default
default:
	@${JMAKE} install

.PHONY : test
test:
	@${JMAKE} run_tests

.PHONY : sonar
sonar:
	@make tidy; rm -rf sonar_output
	@-build-wrapper-macosx-x86 --out-dir sonar_output ${JMAKE}
	@sonar-scanner -Dsonar.login=${SONAR_TOKEN}

#
# print ACE version
#

.PHONY : print-ace-version
print-ace-version:
	@echo ${ACE_VERSION}

# useful macros

BEGIN_DIR = if test -d ${dir}; then ( cd ${dir}
END_DIR   = ) fi

#
# build all Makefiles
#

.PHONY : Makefiles
Makefiles:
	@if [[ -d programs/code_crusader && \
            -d programs/code_medic ]]; \
      then \
         cd programs/code_medic; \
         ${MAKE} -f Make.header update_jcc; \
      fi
	@${foreach dir, \
          ${wildcard lib?* tools/* programs/*} \
          ACE/test tutorial, \
       ${BEGIN_DIR}; makemake; ${JMAKE} Makefiles; ${END_DIR};}
	@cd ../misc; ${JMAKE} Makefiles

#
# build all layouts
#

.PHONY : layouts
layouts:
	@${foreach dir, \
          ${wildcard lib?* tools/* programs/*}, \
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
	@if { ! test -e lib/libACE-*.a; } then \
         cd ACE; ${JMAKE} jxinstall; \
     fi
	@if { test ! -e lib/libreflex.a -a -d misc/reflex; } then \
         cd misc/reflex; ./clean.sh; ./build.sh; \
         cp bin/reflex ${JX_INSTALL_ROOT}; \
     fi
	@if { ! test -x tools/makemake/makemake; } then \
         cd tools/makemake; \
         ${MAKE} -f Makefile.port jxinstall; \
     fi
	@if { ! test -f libjcore/Makefile; } then \
         ${JMAKE} Makefiles; \
     fi

.PHONY: install_libs
install_libs:
	@cd libjcore; ${JMAKE} COMPILE_STRINGS=0 default
	@cd tools/compile_jstrings; ${JMAKE} jxinstall
	@cd libjcore; ${JMAKE} jx.test.skip=true
	@for dir in libjx libjfs libjexpr libj2dplot; do \
       if ! ( cd $$dir; ${JMAKE} default; ); then exit 1; fi \
     done;

.PHONY: install_apps
install_apps:
	@${foreach dir, \
          ${wildcard tools/* programs/*}, \
       ${BEGIN_DIR}; ${JMAKE} jxinstall; ${END_DIR};}

#
# build all libraries
#

.PHONY : libs
libs:
	@${foreach dir, ${wildcard lib?*}, \
       ${BEGIN_DIR}; makemake; ${JMAKE} default; ${END_DIR};}

#
# run all test suites
#

.PHONY : run_tests
run_tests: install_pre install_libs
ifeq (${J_RUN_GCOV},1)
	@find . \( -name '*.gcno' -or -name '*.gcda' \) -and -not -path '*/libjcore/code/*' -exec rm -f '{}' +
	@cd libjcore; p=`pwd`; \
     for f in `find . -name '*.gcno'`; do \
         root=$$p/$${f%/*}; \
         gcov -lp --object-directory $$root $$p/$${f%.*}.o &> /dev/null; \
         mv -f *.gcov $$root &> /dev/null; \
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
          ${addprefix tools/,    ${PERSONAL_TOOLS}} \
          ${addprefix programs/, ${PERSONAL_PROGS}}, \
       ${BEGIN_DIR}; ${JMAKE} personal; ${END_DIR};}

#
# Release
#

RELEASE_PROGS := ${addprefix programs/, \
                   code_crusader code_medic code_mill \
                   systemg ssh_askpass svn_client \
                   drakon leibnitz }
				# glove

.PHONY : release
release:
	@${foreach dir, ${RELEASE_PROGS}, \
       ${BEGIN_DIR}; ${JMAKE} release; ${END_DIR}; }

#
# clean up
#

.PHONY : tidy
tidy:
	@${foreach dir, \
          ${wildcard lib?* tools/* programs/*} \
          ACE misc tutorial, \
       ${BEGIN_DIR}; ${MAKE} tidy; ${END_DIR};}

.PHONY : clean
clean:
	@${foreach dir, \
          ${wildcard lib?* tools/* programs/*} \
          ACE misc tutorial, \
       ${BEGIN_DIR}; ${MAKE} clean; ${END_DIR};}

.PHONY : uninstall
uninstall:
	@${foreach dir, \
          ${wildcard lib?* tools/* programs/*} \
          ACE, \
       ${BEGIN_DIR}; ${MAKE} jxuninstall; ${END_DIR};}
