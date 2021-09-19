/******************************************************************************
 cmGlobals.h

	Copyright (C) 1997 by Glenn W. Bach.

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

const JString&		CMGetVersionNumberStr();
JString				CMGetVersionStr();

bool				CMIsShuttingDown();

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

bool	CMCreateGlobals(CMApp* app);
void	CMCreateCommandDirector();
void	CMDeleteGlobals();
void	CMCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	CMGetWMClassInstance();
const JUtf8Byte*	CMGetCommandWindowClass();
const JUtf8Byte*	CMGetSourceViewWindowClass();
const JUtf8Byte*	CMGetAsmViewWindowClass();
const JUtf8Byte*	CMGetMainSourceWindowClass();
const JUtf8Byte*	CMGetMainAsmWindowClass();
const JUtf8Byte*	CMGetThreadWindowClass();
const JUtf8Byte*	CMGetStackWindowClass();
const JUtf8Byte*	CMGetBreakpointsWindowClass();
const JUtf8Byte*	CMGetVariableWindowClass();
const JUtf8Byte*	CMGetLocalVariableWindowClass();
const JUtf8Byte*	CMGetArray1DWindowClass();
const JUtf8Byte*	CMGetArray2DWindowClass();
const JUtf8Byte*	CMGetPlot2DWindowClass();
const JUtf8Byte*	CMGetFileListWindowClass();
const JUtf8Byte*	CMGetMemoryWindowClass();
const JUtf8Byte*	CMGetRegistersWindowClass();
const JUtf8Byte*	CMGetDebugWindowClass();

// CBCtagsUser

bool	CBInUpdateThread();

#endif
