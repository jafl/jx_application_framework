# Edit this file to define constants and custom build targets.
# Please refer to the makemake documentation for more information.

# Useful directories

MYCODEDIR := .

# Directories to search for header files

SEARCHDIRS := -I- -I${MYCODEDIR}

# makemake variables

LINKER       := gcc
DEPENDFLAGS  := -g -Wall -Werror ${SEARCHDIRS}
TOUCHHEADERS := ${MYCODEDIR}/*.h

# make variables

CC        := gcc
CXX       := g++
CCC       := g++
FC        := f77
CFLAGS     = ${DEPENDFLAGS}
CXXFLAGS   = ${DEPENDFLAGS}
FFLAGS     = 
CPPFLAGS   = 
LOADLIBES := -lm
LDFLAGS   := 

%.o : %.cpp
	${CXX} ${CPPFLAGS} -c $< ${CXXFLAGS}

junk:
	@echo arf


# This is what makemake added


# a.out

a.out : ../../code/CBApp.o ../../code/CBClass.o ../../code/CBCCompleter.o ../../code/cbChangeLogText.o ../../code/CBCharActionManager.o ../../code/CBCharActionTable.o ../../code/CBCClass.o ../../code/CBCmdLineInput.o ../../code/CBCompileDialog.o
	${LINKER} ${LDFLAGS} -o $@ ${filter-out %.a %.so, $^} ${LOADLIBES}


# target for making everything

.PHONY : all
all: a.out


# target for removing all object files

.PHONY : tidy
tidy::
	@${RM} core ../../code/CBApp.o ../../code/CBCClass.o ../../code/CBCCompleter.o ../../code/CBCharActionManager.o ../../code/CBCharActionTable.o ../../code/CBClass.o ../../code/CBCmdLineInput.o ../../code/CBCompileDialog.o ../../code/cbChangeLogText.o

# target for removing all object files

.PHONY : clean
clean:: tidy
	@${RM} a.out

# list of all source files

MM_ALL_SOURCES := ../../code/CBApp.cc ../../code/CBCClass.cc ../../code/CBCCompleter.cc ../../code/CBCharActionManager.cc ../../code/CBCharActionTable.cc ../../code/CBClass.cc ../../code/CBCmdLineInput.cc ../../code/CBCompileDialog.cc ../../code/cbChangeLogText.cc


# target for checking a source file

CHECKSYNTAXFILE := ${basename ${filter %${CHECKSTRING}, ${MM_ALL_SOURCES}}}

.PHONY : checksyntax
checksyntax:
  ifneq (${CHECKSYNTAXFILE},)
	@${MAKE} ${addsuffix .o, ${CHECKSYNTAXFILE}}
  else
	@echo No target to make ${CHECKSTRING}
  endif


# target for touching appropriate source files

.PHONY : touch
touch:
	@echo
	@echo Please ignore \"file arguments missing\" errors
	@echo
	@echo   `grep -l ${TOUCHSTRING} ${MM_ALL_SOURCES}`
	@-touch `grep -l ${TOUCHSTRING} ${MM_ALL_SOURCES}`
	@echo
	@echo   `grep -l ${TOUCHSTRING} ${TOUCHHEADERS}`
	@-touch `grep -l ${TOUCHSTRING} ${TOUCHHEADERS}`


# target for calculating dependencies

.PHONY : jdepend
jdepend:
	@makemake -depend Makefile -- ${DEPENDFLAGS} -- ../../code/CBApp.cc ../../code/CBApp.o ../../code/CBCClass.cc ../../code/CBCClass.o ../../code/CBCCompleter.cc ../../code/CBCCompleter.o ../../code/CBCharActionManager.cc ../../code/CBCharActionManager.o ../../code/CBCharActionTable.cc ../../code/CBCharActionTable.o ../../code/CBClass.cc ../../code/CBClass.o ../../code/CBCmdLineInput.cc ../../code/CBCmdLineInput.o ../../code/CBCompileDialog.cc ../../code/CBCompileDialog.o ../../code/cbChangeLogText.cc ../../code/cbChangeLogText.o


# DO NOT DELETE THIS LINE -- makemake depends on it.

