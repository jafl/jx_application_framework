/******************************************************************************
 cmGlobals.h

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_cmGlobals
#define _H_cmGlobals

#include <jXGlobals.h>
#include "CMApp.h"
#include "CMPrefsManager.h"
#include "CMLink.h"		// for convenience

class JXPSPrinter;
class JX2DPlotEPSPrinter;
class JXPTPrinter;
class CMCommandDirector;
class CBFnMenuUpdater;

CMApp*				CMGetApplication();
CMPrefsManager*		CMGetPrefsManager();
JXPTPrinter*		CMGetPTPrinter();
JXPSPrinter*		CMGetPSPrinter();
JX2DPlotEPSPrinter*	CMGetPlotEPSPrinter();
CBFnMenuUpdater*	CMGetFnMenuUpdater();

void				CMStartDebugger();
CMLink*				CMGetLink();
CMCommandDirector*	CMGetCommandDirector();

const JCharacter*	CMGetVersionNumberStr();
JString				CMGetVersionStr();

JBoolean			CMIsShuttingDown();

	// icons

const JXImage*	CMGetCommandLineIcon();
const JXImage*	CMGetCurrentSourceIcon();
const JXImage*	CMGetCurrentAsmIcon();
const JXImage*	CMGetThreadsIcon();
const JXImage*	CMGetStackTraceIcon();
const JXImage*	CMGetBreakpointsIcon();
const JXImage*	CMGetVariablesIcon();
const JXImage*	CMGetLocalVarsIcon();
const JXImage*	CMGetFileListIcon();
const JXImage*	CMGetSourceFileIcon();
const JXImage*	CMGetAsmSourceIcon();
const JXImage*	CMGetArray1DIcon();
const JXImage*	CMGetArray2DIcon();
const JXImage*	CMGetPlot2DIcon();
const JXImage*	CMGetMemoryIcon();
const JXImage*	CMGetRegistersIcon();

	// called by CMApp

JBoolean	CMCreateGlobals(CMApp* app);
void		CMCreateCommandDirector();
void		CMDeleteGlobals();
void		CMCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JCharacter*	CMGetWMClassInstance();
const JCharacter*	CMGetCommandWindowClass();
const JCharacter*	CMGetSourceViewWindowClass();
const JCharacter*	CMGetAsmViewWindowClass();
const JCharacter*	CMGetMainSourceWindowClass();
const JCharacter*	CMGetMainAsmWindowClass();
const JCharacter*	CMGetThreadWindowClass();
const JCharacter*	CMGetStackWindowClass();
const JCharacter*	CMGetVariableWindowClass();
const JCharacter*	CMGetLocalVariableWindowClass();
const JCharacter*	CMGetArray1DWindowClass();
const JCharacter*	CMGetArray2DWindowClass();
const JCharacter*	CMGetPlot2DWindowClass();
const JCharacter*	CMGetFileListWindowClass();
const JCharacter*	CMGetMemoryWindowClass();
const JCharacter*	CMGetRegistersWindowClass();
const JCharacter*	CMGetDebugWindowClass();

// CBCtagsUser

JBoolean	CBInUpdateThread();

#endif
