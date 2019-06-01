# Makefile to compile JX source distribution

JX_ROOT := .
include ${JX_ROOT}/include/make/jx_constants

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_ROOT}

.PHONY : default
default:
	@cd lib; ${JMAKE} install

.PHONY : test
test:
	@cd lib; ${JMAKE} test

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

#
# clean up
#

.PHONY : tidy
tidy:
	@cd lib; ${JMAKE} tidy

.PHONY : clean
clean:
	@cd lib; ${JMAKE} clean

.PHONY : uninstall
uninstall:
	@cd lib; ${JMAKE} uninstall
