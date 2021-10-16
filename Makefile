JX_ROOT := .

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_ROOT}/bin:/usr/local/opt/bison/bin

.PHONY : default
default:
	@mkdir -p ${JX_INSTALL_ROOT}/bin
	@${JMAKE} initial_build

.PHONY : release
release:
	@${JMAKE} build_release

.PHONY : coverage
coverage:
	@${JMAKE} analyze_coverage

# useful macros

IF_DIR    = if [[ -d ${dir} ]]; then ( cd ${dir};
ENDIF_DIR = ) fi;

#
# initial build
#

.PHONY : initial_build
initial_build: initial_build_makemake initial_build_makefiles initial_build_libs_tools

.PHONY : initial_build_makemake
initial_build_makemake:
	@if [[ -d misc/reflex ]]; then \
       echo Please authorize sudo access for installing reflex...; \
       ${SUDO} echo sudo access authorized...; \
       cd misc/reflex; \
       if [[ ! -f lib/libreflex.a ]]; then \
          ./clean.sh; ./build.sh; \
       fi; \
       ${SUDO} ./allinstall.sh; \
     fi
	@if [[ ! -h ACE/ACE_wrappers && ! -e ACE/ACE_wrappers/ace/libACE.a ]]; then \
       cd ACE; ${JMAKE} install; \
     fi
	@if [[ ! -x tools/makemake/makemake ]]; then \
       cd tools/makemake; \
       ${MAKE} -f Makefile.port install; \
     fi

.PHONY : initial_build_makefiles
initial_build_makefiles:
	@if [[ ! -f libjcore/Makefile ]]; then \
       ${JMAKE} -w Makefiles; \
     fi

.PHONY : initial_build_libs_tools
initial_build_libs_tools:
	@cd libjcore; ${JMAKE} COMPILE_STRINGS=0
	@cd tools/compile_jstrings; ${JMAKE} install
	@cd libjcore; ${JMAKE} jx.test.skip=true
	@for dir in libjx libjfs libjexpr libj2dplot; do \
       if ! ( cd $$dir; ${JMAKE}; ); then exit 1; fi \
     done;
	@$(foreach dir, $(wildcard tools/*), \
       ${IF_DIR} ${JMAKE}; ${ENDIF_DIR})

#
# build all Makefiles
#

.PHONY : Makefiles
Makefiles:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE/test tutorial, \
       ${IF_DIR} makemake; ${JMAKE} Makefiles; ${ENDIF_DIR})

#
# build all libraries
#

.PHONY : libs
libs:
	@$(foreach dir, $(wildcard lib?*), \
       ${IF_DIR} makemake; ${JMAKE}; ${ENDIF_DIR})

#
# build code coverage report
#

.PHONY : analyze_coverage
analyze_coverage: initial_build_makemake
	@cd libjcore; \
     makemake; ${JMAKE} Makefiles; \
     ${JMAKE} \
         J_GCC_LIBS="${J_GCC_LIBS} -coverage" \
         J_COMPILER_DEPEND_FLAGS="${J_COMPILER_DEPEND_FLAGS} -coverage" \
         COMPILE_STRINGS=0
	@${RM} libjcore/test/code/*.gcno libjcore/test/code/*.gcda;
	@cd libjcore; p=`pwd`; \
     for f in `find . -name '*.gcno'`; do \
         root=$$p/$${f%/*}; \
         gcov -lp --object-directory $$root $$p/$${f%.*}.o; \
         mv -f *.gcov $$root; \
     done

#  &> /dev/null

#
# build all layouts
#

.PHONY : layouts
layouts:
	@$(foreach dir, $(wildcard lib?* tools/*), \
       ${IF_DIR} \
           if compgen -G "*.fd" > /dev/null; then \
               jxlayout --require-obj-names *.fd; \
           fi; \
       ${ENDIF_DIR})

#
# install libraries, headers, etc.
#

.PHONY : install
install:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE, \
       ${IF_DIR} ${JMAKE} install; ${ENDIF_DIR})
	@cp -RL ${MAKE_INCLUDE} ${JX_ROOT}/include/jx-af/scripts ${JX_INSTALL_ROOT}/include/jx-af

.PHONY : uninstall
uninstall:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE, \
       ${IF_DIR} ${MAKE} uninstall; ${ENDIF_DIR})
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
	@cd release_pkg; mkdir -p usr/local DEBIAN; mv bin lib include etc usr/local;
	@cp release/pkg/jx_application_framework.debctrl release_pkg/DEBIAN/control
	@perl -pi -e 's/%VERSION%/${JX_VERSION}/' release_pkg/DEBIAN/control;
	@perl -pi -e 's/%ARCH%/${DEB_ARCH}/' release_pkg/DEBIAN/control
	@dpkg-deb --build release_pkg
	@mv release_pkg.deb ../jx-application-framework_${JX_VERSION}_${DEB_ARCH}.deb
	@cd release_pkg; mv usr/local/* .; ${RM} -r usr/local DEBIAN

	@${RM} -r reflex_pkg; mkdir -p reflex_pkg/usr/local/include reflex_pkg/usr/local/lib reflex_pkg/DEBIAN
	@cp -RL /usr/local/include/reflex reflex_pkg/usr/local/include
	@cp -RL /usr/local/lib/libreflex.* reflex_pkg/usr/local/lib
	@cp release/pkg/re-flex.debctrl reflex_pkg/DEBIAN/control
	@perl -pi -e 's/%VERSION%/${REFLEX_VERSION}/' reflex_pkg/DEBIAN/control;
	@perl -pi -e 's/%ARCH%/${DEB_ARCH}/' reflex_pkg/DEBIAN/control
	@dpkg-deb --build reflex_pkg
	@mv reflex_pkg.deb ../re-flex_${JX_VERSION}_${DEB_ARCH}.deb
	@${RM} -r reflex_pkg
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
       ${IF_DIR} ${MAKE} tidy; ${ENDIF_DIR})

.PHONY : clean
clean:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE tutorial, \
       ${IF_DIR} ${MAKE} clean; ${ENDIF_DIR})
