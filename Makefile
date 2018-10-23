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
