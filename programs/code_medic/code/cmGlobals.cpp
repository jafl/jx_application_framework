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
static JBoolean				theShutdownFlag     = kJFalse;
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

static const JCharacter* kCommandWindowClass     = "Code_Medic_Command_Line";
static const JCharacter* kSourceWindowClass      = "Code_Medic_Source";
static const JCharacter* kAsmWindowClass         = "Code_Medic_Disassembly";
static const JCharacter* kMainSourceWindowClass  = "Code_Medic_Source_Main";
static const JCharacter* kMainAsmWindowClass     = "Code_Medic_Disassembly_Main";
static const JCharacter* kThreadWindowClass      = "Code_Medic_Threads";
static const JCharacter* kStackWindowClass       = "Code_Medic_Stack";
static const JCharacter* kBreakpointsWindowClass = "Code_Medic_Breakpoints";
static const JCharacter* kVariableWindowClass    = "Code_Medic_Variables";
static const JCharacter* kLocalVarsWindowClass   = "Code_Medic_Variables_Local";
static const JCharacter* kArray1DWindowClass     = "Code_Medic_Variables_Array_1D";
static const JCharacter* kArray2DWindowClass     = "Code_Medic_Variables_Array_2D";
static const JCharacter* kPlot2DWindowClass      = "Code_Medic_Variables_Plot_2D";
static const JCharacter* kFileListWindowClass    = "Code_Medic_File_List";
static const JCharacter* kMemoryWindowClass      = "Code_Medic_Memory";
static const JCharacter* kRegistersWindowClass   = "Code_Medic_Registers";
static const JCharacter* kDebugWindowClass       = "Code_Medic_Debug";

// private functions

void	CMCreateIcons();
void	CMDeleteIcons();

/******************************************************************************
 CMCreateGlobals

 ******************************************************************************/

JBoolean
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
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, ".gMedic.pref");
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
			{
			JRenameFile(oldPrefsFile, newPrefsFile);
			}
		}

	JBoolean isNew;
	thePrefsManager = jnew CMPrefsManager(&isNew);
	assert(thePrefsManager != nullptr);

	JXInitHelp();

	JXWDManager* wdMgr = jnew JXWDManager(display, kJTrue);
	assert( wdMgr != nullptr );

	CMDockManager* dockManager = jnew CMDockManager;
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

	thePrefsManager->SyncWithCodeCrusader();
	thePrefsManager->LoadSearchPrefs();		// requires JXHelpManager

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
	theShutdownFlag = kJTrue;

	(JXGetDockManager())->JPrefObject::WritePrefs();

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

JBoolean
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

const JCharacter*
CMGetWMClassInstance()
{
	return JGetString("CMName");
}

const JCharacter*
CMGetCommandWindowClass()
{
	return kCommandWindowClass;
}

const JCharacter*
CMGetSourceViewWindowClass()
{
	return kSourceWindowClass;
}

const JCharacter*
CMGetAsmViewWindowClass()
{
	return kAsmWindowClass;
}

const JCharacter*
CMGetMainSourceWindowClass()
{
	return kMainSourceWindowClass;
}

const JCharacter*
CMGetMainAsmWindowClass()
{
	return kMainAsmWindowClass;
}

const JCharacter*
CMGetThreadWindowClass()
{
	return kThreadWindowClass;
}

const JCharacter*
CMGetStackWindowClass()
{
	return kStackWindowClass;
}

const JCharacter*
CMGetBreakpointsWindowClass()
{
	return kBreakpointsWindowClass;
}

const JCharacter*
CMGetVariableWindowClass()
{
	return kVariableWindowClass;
}

const JCharacter*
CMGetLocalVariableWindowClass()
{
	return kLocalVarsWindowClass;
}

const JCharacter*
CMGetArray1DWindowClass()
{
	return kArray1DWindowClass;
}

const JCharacter*
CMGetArray2DWindowClass()
{
	return kArray2DWindowClass;
}

const JCharacter*
CMGetPlot2DWindowClass()
{
	return kPlot2DWindowClass;
}

const JCharacter*
CMGetFileListWindowClass()
{
	return kFileListWindowClass;
}

const JCharacter*
CMGetMemoryWindowClass()
{
	return kMemoryWindowClass;
}

const JCharacter*
CMGetRegistersWindowClass()
{
	return kRegistersWindowClass;
}

const JCharacter*
CMGetDebugWindowClass()		// required so Enlightenment will allow it to resize
{
	return kDebugWindowClass;
}

/******************************************************************************
 CMGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
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
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString("CMDescription", map, sizeof(map));
}

/******************************************************************************
 CBInUpdateThread

 ******************************************************************************/

JBoolean
CBInUpdateThread()
{
	return kJFalse;
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
