JX_ROOT := .

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_ROOT}

.PHONY : default
default:
	@${JMAKE} initial_build

.PHONY : test
test:
	@${JMAKE} run_tests

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
# initial build
#

.PHONY : initial_build
initial_build:
	@if { test -d misc/reflex; } then \
         echo Please authorize sudo access for building reflex...; \
         ${SUDO} echo sudo access authorized...; \
         cd misc/reflex; ./clean.sh; ./build.sh; sudo ./allinstall.sh; \
     fi
	@if { ! test -e lib/libACE-*.a; } then \
         cd ACE; ${JMAKE} install; \
     fi
	@if { ! test -x tools/makemake/makemake; } then \
         cd tools/makemake; \
         ${MAKE} -f Makefile.port install; \
     fi
	@if { ! test -f libjcore/Makefile; } then \
         ${JMAKE} Makefiles; \
     fi
	@cd libjcore; ${JMAKE} COMPILE_STRINGS=0
	@cd tools/compile_jstrings; ${JMAKE} install
	@cd libjcore; ${JMAKE} jx.test.skip=true
	@for dir in libjx libjfs libjexpr libj2dplot; do \
       if ! ( cd $$dir; ${JMAKE}; ); then exit 1; fi \
     done;
	@${foreach dir, \
          ${wildcard tools/*}, \
       ${BEGIN_DIR}; ${JMAKE} install; ${END_DIR};}

#
# build all Makefiles
#

.PHONY : Makefiles
Makefiles:
	@${foreach dir, \
          ${wildcard lib?* tools/*} ACE/test tutorial, \
       ${BEGIN_DIR}; makemake; ${JMAKE} Makefiles; ${END_DIR};}

#
# build all libraries
#

.PHONY : libs
libs:
	@${foreach dir, ${wildcard lib?*}, \
       ${BEGIN_DIR}; makemake; ${JMAKE}; ${END_DIR};}

#
# run all test suites
#

.PHONY : run_tests
run_tests: libs
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
# build all layouts
#

.PHONY : layouts
layouts:
	@${foreach dir, \
          ${wildcard lib?* tools/*}, \
       ${BEGIN_DIR}; \
           if compgen -G "*.fd" > /dev/null; then \
               jxlayout --require-obj-names *.fd; \
           fi; \
       ${END_DIR};}

#
# Sonar
#

.PHONY : sonar
sonar:
	@make tidy; rm -rf sonar_output
	@-build-wrapper-macosx-x86 --out-dir sonar_output ${JMAKE}
	@sonar-scanner -Dsonar.login=${SONAR_TOKEN}

#
# clean up
#

.PHONY : tidy
tidy:
	@${foreach dir, \
          ${wildcard lib?* tools/*}  ACE tutorial, \
       ${BEGIN_DIR}; ${MAKE} tidy; ${END_DIR};}

.PHONY : clean
clean:
	@${foreach dir, \
          ${wildcard lib?* tools/*} ACE tutorial, \
       ${BEGIN_DIR}; ${MAKE} clean; ${END_DIR};}

.PHONY : uninstall
uninstall:
	@${foreach dir, \
          ${wildcard lib?* tools/*}  ACE, \
       ${BEGIN_DIR}; ${MAKE} uninstall; ${END_DIR};}
