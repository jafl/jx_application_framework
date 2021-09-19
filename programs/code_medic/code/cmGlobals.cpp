/******************************************************************************
 cmGlobals.h

	Copyright (C) 1997-02 by Glenn W. Bach.

 ******************************************************************************/

#include "cmGlobals.h"
#include "CMDockManager.h"
#include "CBFnMenuUpdater.h"
#include "GDBLink.h"
#include "LLDBLink.h"
#include "JVMLink.h"
#include "XDLink.h"
#include "CMCommandDirector.h"
#include <JXWDManager.h>
#include <JXPSPrinter.h>
#include <JX2DPlotEPSPrinter.h>
#include <JXPTPrinter.h>
#include <JXDisplay.h>
#include <JXImageCache.h>
#include <JXImage.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

static CMApp*				theApplication		= nullptr;		// owned by JX
static bool					theShutdownFlag     = false;
static CMPrefsManager*		thePrefsManager		= nullptr;
static JXPTPrinter*			theTextPrinter		= nullptr;
static JXPSPrinter*			thePSPrinter		= nullptr;
static JX2DPlotEPSPrinter*	thePlotEPSPrinter	= nullptr;
static CBFnMenuUpdater*		theFnMenuUpdater	= nullptr;

static CMLink*				theLink   = nullptr;
static CMCommandDirector*	theCmdDir = nullptr;

// owned by JXImageCache
static JXImage* theCommandLineIcon   = nullptr;
static JXImage* theCurrentSourceIcon = nullptr;
static JXImage* theCurrentAsmIcon    = nullptr;
static JXImage* theThreadsIcon       = nullptr;
static JXImage* theStackTraceIcon    = nullptr;
static JXImage* theBreakpointsIcon   = nullptr;
static JXImage* theVariablesIcon     = nullptr;
static JXImage* theLocalVarsIcon     = nullptr;
static JXImage* theFileListIcon      = nullptr;
static JXImage* theSourceFileIcon    = nullptr;
static JXImage* theAsmSourceIcon     = nullptr;
static JXImage* theArray1DIcon       = nullptr;
static JXImage* theArray2DIcon       = nullptr;
static JXImage* thePlot2DIcon        = nullptr;
static JXImage* theMemoryIcon        = nullptr;
static JXImage* theRegistersIcon     = nullptr;

static const JUtf8Byte* kCommandWindowClass     = "Code_Medic_Command_Line";
static const JUtf8Byte* kSourceWindowClass      = "Code_Medic_Source";
static const JUtf8Byte* kAsmWindowClass         = "Code_Medic_Disassembly";
static const JUtf8Byte* kMainSourceWindowClass  = "Code_Medic_Source_Main";
static const JUtf8Byte* kMainAsmWindowClass     = "Code_Medic_Disassembly_Main";
static const JUtf8Byte* kThreadWindowClass      = "Code_Medic_Threads";
static const JUtf8Byte* kStackWindowClass       = "Code_Medic_Stack";
static const JUtf8Byte* kBreakpointsWindowClass = "Code_Medic_Breakpoints";
static const JUtf8Byte* kVariableWindowClass    = "Code_Medic_Variables";
static const JUtf8Byte* kLocalVarsWindowClass   = "Code_Medic_Variables_Local";
static const JUtf8Byte* kArray1DWindowClass     = "Code_Medic_Variables_Array_1D";
static const JUtf8Byte* kArray2DWindowClass     = "Code_Medic_Variables_Array_2D";
static const JUtf8Byte* kPlot2DWindowClass      = "Code_Medic_Variables_Plot_2D";
static const JUtf8Byte* kFileListWindowClass    = "Code_Medic_File_List";
static const JUtf8Byte* kMemoryWindowClass      = "Code_Medic_Memory";
static const JUtf8Byte* kRegistersWindowClass   = "Code_Medic_Registers";
static const JUtf8Byte* kDebugWindowClass       = "Code_Medic_Debug";

// private functions

void	CMCreateIcons();
void	CMDeleteIcons();

/******************************************************************************
 CMCreateGlobals

 ******************************************************************************/

bool
CMCreateGlobals
	(
	CMApp* app
	)
{
	theApplication = app;

	JXDisplay* display = app->GetCurrentDisplay();

	CMCreateIcons();

	JString oldPrefsFile, newPrefsFile;
	if (JGetPrefsDirectory(&oldPrefsFile))
		{
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, JString(".gMedic.pref", JString::kNoCopy));
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
			{
			JRenameFile(oldPrefsFile, newPrefsFile);
			}
		}

	bool isNew;
	thePrefsManager = jnew CMPrefsManager(&isNew);
	assert(thePrefsManager != nullptr);

	JXInitHelp();

	auto* wdMgr = jnew JXWDManager(display, true);
	assert( wdMgr != nullptr );

	auto* dockManager = jnew CMDockManager;
	assert( dockManager != nullptr );
	dockManager->JPrefObject::ReadPrefs();

	theTextPrinter = jnew JXPTPrinter;
	assert( theTextPrinter != nullptr );
	thePrefsManager->ReadPrinterSetup(theTextPrinter);

	thePSPrinter = jnew JXPSPrinter(display);
	assert( thePSPrinter != nullptr );
	thePrefsManager->ReadPrinterSetup(thePSPrinter);

	thePlotEPSPrinter = jnew JX2DPlotEPSPrinter(display);
	assert( thePlotEPSPrinter != nullptr );
	thePrefsManager->ReadPrinterSetup(thePlotEPSPrinter);

	theFnMenuUpdater = jnew CBFnMenuUpdater;
	assert( theFnMenuUpdater != nullptr );

	thePrefsManager->LoadSearchPrefs();
	thePrefsManager->SyncWithCodeCrusader();	// after creating search dialog

	lldb::SBDebugger::Initialize();
	CMStartDebugger();

	return isNew;
}

/******************************************************************************
 CMCreateCommandDirector

 ******************************************************************************/

void
CMCreateCommandDirector()
{
	theCmdDir = jnew CMCommandDirector(theApplication);
	assert( theCmdDir != nullptr );
	theCmdDir->Activate();
}

/******************************************************************************
 CMDeleteGlobals

 ******************************************************************************/

void
CMDeleteGlobals()
{
	theShutdownFlag = true;

	JXGetDockManager()->JPrefObject::WritePrefs();

	thePrefsManager->WritePrinterSetup(theTextPrinter);
	jdelete theTextPrinter;
	theTextPrinter = nullptr;

	thePrefsManager->WritePrinterSetup(thePSPrinter);
	jdelete thePSPrinter;
	thePSPrinter = nullptr;

	thePrefsManager->WritePrinterSetup(thePlotEPSPrinter);
	jdelete thePlotEPSPrinter;
	thePlotEPSPrinter = nullptr;

	jdelete theFnMenuUpdater;
	theFnMenuUpdater = nullptr;

	jdelete theLink;
	theLink = nullptr;
	lldb::SBDebugger::Terminate();

	CMDeleteIcons();
	CBShutdownStylers();

	theApplication = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 CMCleanUpBeforeSuddenDeath

	This must be the last one called by CBApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
CMCleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		CBShutdownStylers();
		}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 CMStartDebugger

 ******************************************************************************/

void
CMStartDebugger()
{
	CMLink* origLink = theLink;
	theLink          = nullptr;

	CMPrefsManager::DebuggerType type = CMGetPrefsManager()->GetDebuggerType();
	if (type == CMPrefsManager::kGDBType)
		{
		theLink = jnew GDBLink;
		}
	else if (type == CMPrefsManager::kLLDBType)
		{
		theLink = jnew LLDBLink;
		}
	else if (type == CMPrefsManager::kJavaType)
		{
		theLink = jnew JVMLink;
		}
	else if (type == CMPrefsManager::kXdebugType)
		{
		theLink = jnew XDLink;
		}
	assert (theLink != nullptr);

	// original must be deleted *last* so listeners can call CMGetLink() to
	// get the new one

	jdelete origLink;

	if (theCmdDir != nullptr)
		{
		theCmdDir->InitializeCommandOutput();
		}
}

/******************************************************************************
 CMIsShuttingDown

 ******************************************************************************/

bool
CMIsShuttingDown()
{
	return theShutdownFlag;
}

/******************************************************************************
 CMGetApplication

 ******************************************************************************/

CMApp*
CMGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 CMGetPrefsManager

 ******************************************************************************/

CMPrefsManager*
CMGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 CMGetPTPrinter

 ******************************************************************************/

JXPTPrinter*
CMGetPTPrinter()
{
	assert( theTextPrinter != nullptr );
	return theTextPrinter;
}

/******************************************************************************
 CMGetPSPrinter

 ******************************************************************************/

JXPSPrinter*
CMGetPSPrinter()
{
	assert( thePSPrinter != nullptr );
	return thePSPrinter;
}

/******************************************************************************
 CMGetPlotEPSPrinter

 ******************************************************************************/

JX2DPlotEPSPrinter*
CMGetPlotEPSPrinter()
{
	assert( thePlotEPSPrinter != nullptr );
	return thePlotEPSPrinter;
}

/******************************************************************************
 CMGetFnMenuUpdater

 ******************************************************************************/

CBFnMenuUpdater*
CMGetFnMenuUpdater()
{
	assert( theFnMenuUpdater != nullptr );
	return theFnMenuUpdater;
}

/******************************************************************************
 CMGetLink

 *****************************************************************************/

CMLink*
CMGetLink()
{
	assert( theLink != nullptr );
	return theLink;
}

/******************************************************************************
 GetCommandDirector

 ******************************************************************************/

CMCommandDirector*
CMGetCommandDirector()
{
	assert( theCmdDir != nullptr );
	return theCmdDir;
}

/******************************************************************************
 CMGetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
CMGetWMClassInstance()
{
	return JGetString("CMName").GetBytes();
}

const JUtf8Byte*
CMGetCommandWindowClass()
{
	return kCommandWindowClass;
}

const JUtf8Byte*
CMGetSourceViewWindowClass()
{
	return kSourceWindowClass;
}

const JUtf8Byte*
CMGetAsmViewWindowClass()
{
	return kAsmWindowClass;
}

const JUtf8Byte*
CMGetMainSourceWindowClass()
{
	return kMainSourceWindowClass;
}

const JUtf8Byte*
CMGetMainAsmWindowClass()
{
	return kMainAsmWindowClass;
}

const JUtf8Byte*
CMGetThreadWindowClass()
{
	return kThreadWindowClass;
}

const JUtf8Byte*
CMGetStackWindowClass()
{
	return kStackWindowClass;
}

const JUtf8Byte*
CMGetBreakpointsWindowClass()
{
	return kBreakpointsWindowClass;
}

const JUtf8Byte*
CMGetVariableWindowClass()
{
	return kVariableWindowClass;
}

const JUtf8Byte*
CMGetLocalVariableWindowClass()
{
	return kLocalVarsWindowClass;
}

const JUtf8Byte*
CMGetArray1DWindowClass()
{
	return kArray1DWindowClass;
}

const JUtf8Byte*
CMGetArray2DWindowClass()
{
	return kArray2DWindowClass;
}

const JUtf8Byte*
CMGetPlot2DWindowClass()
{
	return kPlot2DWindowClass;
}

const JUtf8Byte*
CMGetFileListWindowClass()
{
	return kFileListWindowClass;
}

const JUtf8Byte*
CMGetMemoryWindowClass()
{
	return kMemoryWindowClass;
}

const JUtf8Byte*
CMGetRegistersWindowClass()
{
	return kRegistersWindowClass;
}

const JUtf8Byte*
CMGetDebugWindowClass()		// required so Enlightenment will allow it to resize
{
	return kDebugWindowClass;
}

/******************************************************************************
 CMGetVersionNumberStr

 ******************************************************************************/

const JString&
CMGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 CMGetVersionStr

 ******************************************************************************/

JString
CMGetVersionStr()
{
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("CMDescription", map, sizeof(map));
}

/******************************************************************************
 CBInUpdateThread

 ******************************************************************************/

bool
CBInUpdateThread()
{
	return false;
}

/******************************************************************************
 Icons

 ******************************************************************************/

#include "medic_show_command_line.xpm"
#include "medic_show_current_source.xpm"
#include "medic_show_current_asm.xpm"
#include "medic_show_threads.xpm"
#include "medic_show_stack_trace.xpm"
#include "medic_show_breakpoints.xpm"
#include "medic_show_variables.xpm"
#include "medic_show_local_variables.xpm"
#include "medic_show_file_list.xpm"
#include <jx_source_file_small.xpm>
#include <jx_binary_file_small.xpm>
#include "medic_show_1d_array.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_memory.xpm"
#include "medic_show_registers.xpm"

void
CMCreateIcons()
{
	JXImageCache* c = theApplication->GetCurrentDisplay()->GetImageCache();

	theCommandLineIcon   = c->GetImage(medic_show_command_line);
	theCurrentSourceIcon = c->GetImage(medic_show_current_source);
	theCurrentAsmIcon    = c->GetImage(medic_show_current_asm);
	theThreadsIcon       = c->GetImage(medic_show_threads);
	theStackTraceIcon    = c->GetImage(medic_show_stack_trace);
	theBreakpointsIcon   = c->GetImage(medic_show_breakpoints);
	theVariablesIcon     = c->GetImage(medic_show_variables);
	theLocalVarsIcon     = c->GetImage(medic_show_local_variables);
	theFileListIcon      = c->GetImage(medic_show_file_list);
	theSourceFileIcon    = c->GetImage(jx_source_file_small);
	theAsmSourceIcon     = c->GetImage(jx_binary_file_small);
	theArray1DIcon       = c->GetImage(medic_show_1d_array);
	theArray2DIcon       = c->GetImage(medic_show_2d_array);
	thePlot2DIcon        = c->GetImage(medic_show_2d_plot);
	theMemoryIcon        = c->GetImage(medic_show_memory);
	theRegistersIcon     = c->GetImage(medic_show_registers);
}

void
CMDeleteIcons()
{
}

const JXImage*
CMGetCommandLineIcon()
{
	return theCommandLineIcon;
}

const JXImage*
CMGetCurrentSourceIcon()
{
	return theCurrentSourceIcon;
}

const JXImage*
CMGetCurrentAsmIcon()
{
	return theCurrentAsmIcon;
}

const JXImage*
CMGetThreadsIcon()
{
	return theThreadsIcon;
}

const JXImage*
CMGetStackTraceIcon()
{
	return theStackTraceIcon;
}

const JXImage*
CMGetBreakpointsIcon()
{
	return theBreakpointsIcon;
}

const JXImage*
CMGetVariablesIcon()
{
	return theVariablesIcon;
}

const JXImage*
CMGetLocalVarsIcon()
{
	return theLocalVarsIcon;
}

const JXImage*
CMGetFileListIcon()
{
	return theFileListIcon;
}

const JXImage*
CMGetSourceFileIcon()
{
	return theSourceFileIcon;
}

const JXImage*
CMGetAsmSourceIcon()
{
	return theAsmSourceIcon;
}

const JXImage*
CMGetArray1DIcon()
{
	return theArray1DIcon;
}

const JXImage*
CMGetArray2DIcon()
{
	return theArray2DIcon;
}

const JXImage*
CMGetPlot2DIcon()
{
	return thePlot2DIcon;
}

const JXImage*
CMGetMemoryIcon()
{
	return theMemoryIcon;
}

const JXImage*
CMGetRegistersIcon()
{
	return theRegistersIcon;
}
