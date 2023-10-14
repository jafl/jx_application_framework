JX_ROOT := .

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_BIN}:/usr/local/opt/bison/bin:/opt/homebrew/opt/bison/bin

.PHONY : default
default:
	@mkdir -p ${JX_INSTALL_BIN}
	@${JMAKE} initial_build

.PHONY : release
release:
	@${JMAKE} build_release

.PHONY : update_docs
update_docs:
	@scp -r doc/html/* jafl,libjx@web.sourceforge.net:htdocs/;
	@scp -r tools/jx_memory_debugger/release/doc/* jafl,libjx@web.sourceforge.net:htdocs/jx_memory_debugger/;
	@scp -r tools/jx_project_wizard/release/doc/* jafl,libjx@web.sourceforge.net:htdocs/jx_project_wizard/;

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
       if ! command -v reflex; then \
         echo Please authorize sudo access for installing reflex...; \
         ${SUDO} echo sudo access authorized...; \
       fi; \
       cd misc/reflex; \
       if [[ ! -f lib/libreflex.a ]]; then \
         ./clean.sh; ./build.sh; \
       fi; \
       if ! command -v reflex; then \
         ${SUDO} ./allinstall.sh; \
       fi; \
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
       ${IF_DIR} ${JMAKE} install; ${ENDIF_DIR})
	@cd tutorial; ${JMAKE}

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
     make clean; \
     ${JMAKE} \
         J_GCC_LIBS="${J_GCC_LIBS} -coverage" \
         J_COMPILER_DEPEND_FLAGS="${J_COMPILER_DEPEND_FLAGS} -coverage" \
         COMPILE_STRINGS=0
	@cd libjcore; gcov -lp code/*.o; mv code\#* code; \
     cd test; gcov -lp code/*.o; mv code\#* code

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
	@cp -RL ${MAKE_INCLUDE} ${JX_ROOT}/include/jx-af/scripts ${JX_INSTALL_INC}
  ifdef SNAPCRAFT_PART_INSTALL
	@cp ${JX_ROOT}/misc/reflex/bin/reflex ${JX_INSTALL_BIN}
	@cp ${JX_ROOT}/misc/reflex/lib/*.a ${JX_INSTALL_LIB}
	@cp -R ${JX_ROOT}/misc/reflex/include/reflex ${JX_INSTALL_INC}/..
  endif

.PHONY : uninstall
uninstall:
	@$(foreach dir, $(wildcard lib?* tools/*) ACE, \
       ${IF_DIR} ${MAKE} uninstall; ${ENDIF_DIR})
	@${RM} -r ${JX_INSTALL_INC}

#
# build packages
#

PKG_PATH        := jx-application-framework-${JX_VERSION}
SOURCE_TAR_NAME := jx-application-framework_${JX_VERSION}_${SYS_NAME}_${SYS_ARCH}.tar
RPM_PKG_NAME    := jx-application-framework-${JX_VERSION}.${SYS_NAME}.${SYS_ARCH}.rpm
DEB_PKG_NAME    := jx-application-framework_${JX_VERSION}_${SYS_NAME}_${SYS_ARCH}.deb
REFLEX_DEB_NAME := re-flex_${REFLEX_VERSION}_${SYS_ARCH}.deb

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

	@${RM} -r ${PKG_PATH}; mkdir -p ${PKG_PATH}
	@pushd ${PKG_PATH}; P=`pwd`; popd; \
     ${MAKE} JX_INSTALL_ROOT=$$P install
	@cp -RL ${MAKE_INCLUDE} ${PKG_PATH}/include/jx-af/; \
     ${RM} -r ${PKG_PATH}/include/jx-af/make/sys
	@cp configure Brewfile release/pkg/install release/pkg/uninstall ${PKG_PATH}
	@${TAR} -chf ../${SOURCE_TAR_NAME} ${PKG_PATH}

  ifeq (${HAS_RPM},1)
	@${SUDO} mkdir -p -m 755 ${RPM_BUILD_DIR} ${RPM_SRC_DIR} ${RPM_SPEC_DIR} ${RPM_BIN_DIR} ${RPM_SRPM_DIR}
	@${SUDO} ./release/pkg/uninstall ${RPM_BUILD_ROOT}/usr/local
	@${SUDO} cp ../${SOURCE_TAR_NAME} ${RPM_SRC_DIR}/${SOURCE_TAR_NAME}
	@${SUDO} ${RPM} --define '_topdir ${RPM_SRC_ROOT}' \
                    --define 'pkg_version ${JX_VERSION}' \
                    --define 'pkg_name ${SOURCE_TAR_NAME}' \
                    ./release/pkg/jx-application-framework.spec
	@${SUDO} mv ${RPM_BIN_DIR}/*/jx-application-framework-*.rpm ../${RPM_PKG_NAME}
	@${SUDO} chown ${USER}. ../*.rpm
	@${SUDO} ./release/pkg/uninstall ${RPM_BUILD_ROOT}/usr/local
  endif

  ifeq (${HAS_DEB},1)
	@cd ${PKG_PATH}; mkdir -p usr/local DEBIAN; mv bin lib include usr/local;
	@cp release/pkg/jx-application-framework.debctrl ${PKG_PATH}/DEBIAN/control
	@perl -pi -e 's/%VERSION%/${JX_VERSION}/' ${PKG_PATH}/DEBIAN/control;
	@perl -pi -e 's/%ARCH%/${SYS_ARCH}/' ${PKG_PATH}/DEBIAN/control
	@dpkg-deb --build ${PKG_PATH}
	@mv ${PKG_PATH}.deb ../${DEB_PKG_NAME}
	@cd ${PKG_PATH}; mv usr/local/* .; ${RM} -r usr/local DEBIAN

	@${RM} -r reflex_pkg; mkdir -p reflex_pkg/usr/local/lib reflex_pkg/usr/local/include reflex_pkg/usr/local/bin reflex_pkg/DEBIAN
	@cp /usr/local/lib/libreflex.* reflex_pkg/usr/local/lib
	@cp -RL /usr/local/include/reflex reflex_pkg/usr/local/include
	@cp /usr/local/bin/reflex reflex_pkg/usr/local/bin
	@cp release/pkg/re-flex.debctrl reflex_pkg/DEBIAN/control
	@perl -pi -e 's/%VERSION%/${REFLEX_VERSION}/' reflex_pkg/DEBIAN/control;
	@perl -pi -e 's/%ARCH%/${SYS_ARCH}/' reflex_pkg/DEBIAN/control
	@dpkg-deb --build reflex_pkg
	@mv reflex_pkg.deb ../${REFLEX_DEB_NAME}
	@${RM} -r reflex_pkg
  endif

	@gzip ../${SOURCE_TAR_NAME}
	@${RM} -r ${PKG_PATH}

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
