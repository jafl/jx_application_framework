JX_ROOT := .

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_ROOT}/bin:/usr/local/opt/bison/bin

.PHONY : default
default:
	@mkdir -p ${JX_INSTALL_ROOT}/bin
	@${JMAKE} initial_build

.PHONY : test
test:
	@${JMAKE} run_tests

.PHONY : release
release:
	@${JMAKE} build_release

# useful macros

BEGIN_DIR = if { test -d ${dir}; } then ( cd ${dir}
END_DIR   = ) fi

#
# initial build
#

.PHONY : initial_build
initial_build:
	@if { test -d misc/reflex; } then \
       echo Please authorize sudo access for installing reflex...; \
       ${SUDO} echo sudo access authorized...; \
       cd misc/reflex; \
       if { ! test -f lib/libreflex.a;  } then \
          ./clean.sh; ./build.sh; \
       fi; \
       ${SUDO} ./allinstall.sh; \
     fi
	@if { ! test -h ACE/ACE_wrappers && ! test -e ACE/ACE_wrappers/ace/libACE.a; } then \
       cd ACE; ${JMAKE} install; \
     fi
	@if { ! test -x tools/makemake/makemake; } then \
       cd tools/makemake; \
       ${MAKE} -f Makefile.port install; \
     fi
	@if { ! test -f libjcore/Makefile; } then \
       ${JMAKE} -w Makefiles; \
     fi
	@cd libjcore; ${JMAKE} COMPILE_STRINGS=0
	@cd tools/compile_jstrings; ${JMAKE} install
	@cd libjcore; ${JMAKE} jx.test.skip=true
	@for dir in libjx libjfs libjexpr libj2dplot; do \
       if ! ( cd $$dir; ${JMAKE}; ); then exit 1; fi \
     done;
	@$(foreach dir, $(wildcard tools/*), \
       ${BEGIN_DIR}; ${JMAKE}; ${END_DIR};)

#
# build all Makefiles
#

.PHONY : Makefiles
Makefiles:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE/test tutorial, \
       ${BEGIN_DIR}; makemake; ${JMAKE} Makefiles; ${END_DIR};)

#
# build all libraries
#

.PHONY : libs
libs:
	@$(foreach dir, $(wildcard lib?*), \
       ${BEGIN_DIR}; makemake; ${JMAKE}; ${END_DIR};)

#
# run all test suites
#

.PHONY : run_tests
run_tests: libs
ifeq (${J_RUN_GCOV},1)
	@mkdir -p .sonar/cache .scannerwork
	@chmod -R 777 .sonar .scannerwork
	@find . \( -name '*.gcno' -or -name '*.gcda' \) -and -not -path '*/libjcore/code/*' -exec ${RM} '{}' +
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
	@$(foreach dir, $(wildcard lib?* tools/*), \
       ${BEGIN_DIR}; \
           if compgen -G "*.fd" > /dev/null; then \
               jxlayout --require-obj-names *.fd; \
           fi; \
       ${END_DIR};)

#
# install libraries, headers, etc.
#

.PHONY : install
install:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE, \
       ${BEGIN_DIR}; ${MAKE} install; ${END_DIR};)
	@cp -RL ${MAKE_INCLUDE} ${JX_INSTALL_ROOT}/include/jx-af

.PHONY : uninstall
uninstall:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE, \
       ${BEGIN_DIR}; ${MAKE} uninstall; ${END_DIR};)
	@${RM} -r ${JX_INSTALL_ROOT}/include/jx-af

#
# build packages
#

.PHONY : build_release
build_release:
  ifneq (${J_IS_RELEASE_BUILD},1)
	@echo You can only build a package when you './configure release'
	@false
  endif

  ifeq (${HAS_RPM},1)
	@echo Please authorize sudo access for building RPM...
	@${SUDO} echo sudo access authorized...
  endif

	@${RM} -r release_pkg; mkdir -p release_pkg
	@pushd release_pkg; export JX_INSTALL_ROOT=`pwd`; popd; \
     ${MAKE} install
	@cp -RL ${MAKE_INCLUDE} release_pkg/include/jx-af/; \
     ${RM} -r release_pkg/include/jx-af/make/sys

  ifeq (${HAS_RPM},1)
	@${SUDO} mkdir -p -m 755 ${RPM_BUILD_DIR} ${RPM_SRC_DIR} ${RPM_SPEC_DIR} ${RPM_BIN_DIR} ${RPM_SRPM_DIR}
	@${SUDO} ./release/pkg/uninstall
	@${SUDO} cp ../${SOURCE_TAR_NAME} ${RPM_SRC_DIR}/${SOURCE_TAR_NAME}
	@${SUDO} ${RPM} --define '_topdir ${RPM_SRC_ROOT}' \
                    --define 'pkg_version ${APP_VERSION}' ./release/pkg/jx_application_framework.spec
	@${SUDO} mv ${RPM_BIN_DIR}/*/*.rpm ../
	@${SUDO} chown ${USER}. ../*.rpm
	@${SUDO} ./release/pkg/uninstall
  endif

  ifeq (${HAS_DEB},1)
	@cd release_pkg; mkdir -p usr/local; mv bin lib include etc usr/local;
	@${RM} -r release_pkg/DEBIAN; mkdir -p release_pkg/DEBIAN
	@cp release/pkg/jx_application_framework.debctrl release_pkg/DEBIAN/control
	@perl -pi -e 's/%VERSION%/${JX_VERSION}/' release_pkg/DEBIAN/control;
	@perl -pi -e 's/%ARCH%/'`dpkg --print-architecture`'/' release_pkg/DEBIAN/control
	@dpkg-deb --build release_pkg
	@mv release_pkg.deb jx-application-framework.deb
	@cd release_pkg; mv usr/local/* .; ${RM} -r usr/local DEBIAN
  endif

	@${RM} -r release_pkg

#
# Sonar
#

.PHONY : sonar
sonar:
	@make tidy; ${RM} -r sonar_output
	@-build-wrapper-macosx-x86 --out-dir sonar_output ${JMAKE}
	@sonar-scanner -Dsonar.login=${SONAR_TOKEN}

#
# clean up
#

.PHONY : tidy
tidy:
	@$(foreach dir, $(wildcard lib?* tools/*)  ACE tutorial, \
       ${BEGIN_DIR}; ${MAKE} tidy; ${END_DIR};)

.PHONY : clean
clean:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE tutorial, \
       ${BEGIN_DIR}; ${MAKE} clean; ${END_DIR};)
