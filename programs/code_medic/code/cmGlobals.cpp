/******************************************************************************
 cmGlobals.h

	Copyright © 1997-02 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "cmGlobals.h"
#include "cmHelpText.h"
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

static CMApp*				theApplication		= NULL;		// owned by JX
static JBoolean				theShutdownFlag     = kJFalse;
static CMPrefsManager*		thePrefsManager		= NULL;
static JXPTPrinter*			theTextPrinter		= NULL;
static JXPSPrinter*			thePSPrinter		= NULL;
static JX2DPlotEPSPrinter*	thePlotEPSPrinter	= NULL;
static CBFnMenuUpdater*		theFnMenuUpdater	= NULL;

static CMLink*				theLink   = NULL;
static CMCommandDirector*	theCmdDir = NULL;

// owned by JXImageCache
static JXImage* theCommandLineIcon   = NULL;
static JXImage* theCurrentSourceIcon = NULL;
static JXImage* theCurrentAsmIcon    = NULL;
static JXImage* theThreadsIcon       = NULL;
static JXImage* theStackTraceIcon    = NULL;
static JXImage* theBreakpointsIcon   = NULL;
static JXImage* theVariablesIcon     = NULL;
static JXImage* theLocalVarsIcon     = NULL;
static JXImage* theFileListIcon      = NULL;
static JXImage* theSourceFileIcon    = NULL;
static JXImage* theAsmSourceIcon     = NULL;
static JXImage* theArray1DIcon       = NULL;
static JXImage* theArray2DIcon       = NULL;
static JXImage* thePlot2DIcon        = NULL;
static JXImage* theMemoryIcon        = NULL;
static JXImage* theRegistersIcon     = NULL;

static const JCharacter* kCommandWindowClass    = "Code_Medic_Command_Line";
static const JCharacter* kSourceWindowClass     = "Code_Medic_Source";
static const JCharacter* kAsmWindowClass        = "Code_Medic_Disassembly";
static const JCharacter* kMainSourceWindowClass = "Code_Medic_Source_Main";
static const JCharacter* kMainAsmWindowClass    = "Code_Medic_Disassembly_Main";
static const JCharacter* kThreadWindowClass     = "Code_Medic_Threads";
static const JCharacter* kStackWindowClass      = "Code_Medic_Stack";
static const JCharacter* kVariableWindowClass   = "Code_Medic_Variables";
static const JCharacter* kLocalVarsWindowClass  = "Code_Medic_Variables_Local";
static const JCharacter* kArray1DWindowClass    = "Code_Medic_Variables_Array_1D";
static const JCharacter* kArray2DWindowClass    = "Code_Medic_Variables_Array_2D";
static const JCharacter* kPlot2DWindowClass     = "Code_Medic_Variables_Plot_2D";
static const JCharacter* kFileListWindowClass   = "Code_Medic_File_List";
static const JCharacter* kMemoryWindowClass     = "Code_Medic_Memory";
static const JCharacter* kRegistersWindowClass  = "Code_Medic_Registers";
static const JCharacter* kDebugWindowClass      = "Code_Medic_Debug";

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
	thePrefsManager = new CMPrefsManager(&isNew);
	assert(thePrefsManager != NULL);

	JXInitHelp(kCMTOCHelpName, kCMHelpSectionCount, kCMHelpSectionName);

	JXWDManager* wdMgr = new JXWDManager(display, kJTrue);
	assert( wdMgr != NULL );

	CMDockManager* dockManager = new CMDockManager;
	assert( dockManager != NULL );
	dockManager->JPrefObject::ReadPrefs();

	theTextPrinter = new JXPTPrinter;
	assert( theTextPrinter != NULL );
	thePrefsManager->ReadPrinterSetup(theTextPrinter);

	thePSPrinter = new JXPSPrinter(display);
	assert( thePSPrinter != NULL );
	thePrefsManager->ReadPrinterSetup(thePSPrinter);

	thePlotEPSPrinter = new JX2DPlotEPSPrinter(display);
	assert( thePlotEPSPrinter != NULL );
	thePrefsManager->ReadPrinterSetup(thePlotEPSPrinter);

	theFnMenuUpdater = new CBFnMenuUpdater;
	assert( theFnMenuUpdater != NULL );

	thePrefsManager->SyncWithCodeCrusader();

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
	theCmdDir = new CMCommandDirector(theApplication);
	assert( theCmdDir != NULL );
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
	delete theTextPrinter;
	theTextPrinter = NULL;

	thePrefsManager->WritePrinterSetup(thePSPrinter);
	delete thePSPrinter;
	thePSPrinter = NULL;

	thePrefsManager->WritePrinterSetup(thePlotEPSPrinter);
	delete thePlotEPSPrinter;
	thePlotEPSPrinter = NULL;

	delete theFnMenuUpdater;
	theFnMenuUpdater = NULL;

	delete theLink;
	theLink = NULL;
	lldb::SBDebugger::Terminate();

	CMDeleteIcons();
	CBShutdownStylers();

	theApplication = NULL;

	// this must be last so everybody else can use it to save their setup

	delete thePrefsManager;
	thePrefsManager = NULL;
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
	theLink          = NULL;

	CMPrefsManager::DebuggerType type = CMGetPrefsManager()->GetDebuggerType();
	if (type == CMPrefsManager::kGDBType)
		{
		theLink = new GDBLink;
		}
	else if (type == CMPrefsManager::kLLDBType)
		{
		theLink = new LLDBLink;
		}
	else if (type == CMPrefsManager::kJavaType)
		{
		theLink = new JVMLink;
		}
	else if (type == CMPrefsManager::kXdebugType)
		{
		theLink = new XDLink;
		}
	assert (theLink != NULL);

	// original must be deleted *last* so listeners can call CMGetLink() to
	// get the new one

	delete origLink;

	if (theCmdDir != NULL)
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
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 CMGetPrefsManager

 ******************************************************************************/

CMPrefsManager*
CMGetPrefsManager()
{
	assert( thePrefsManager != NULL );
	return thePrefsManager;
}

/******************************************************************************
 CMGetPTPrinter

 ******************************************************************************/

JXPTPrinter*
CMGetPTPrinter()
{
	assert( theTextPrinter != NULL );
	return theTextPrinter;
}

/******************************************************************************
 CMGetPSPrinter

 ******************************************************************************/

JXPSPrinter*
CMGetPSPrinter()
{
	assert( thePSPrinter != NULL );
	return thePSPrinter;
}

/******************************************************************************
 CMGetPlotEPSPrinter

 ******************************************************************************/

JX2DPlotEPSPrinter*
CMGetPlotEPSPrinter()
{
	assert( thePlotEPSPrinter != NULL );
	return thePlotEPSPrinter;
}

/******************************************************************************
 CMGetFnMenuUpdater

 ******************************************************************************/

CBFnMenuUpdater*
CMGetFnMenuUpdater()
{
	assert( theFnMenuUpdater != NULL );
	return theFnMenuUpdater;
}

/******************************************************************************
 CMGetLink

 *****************************************************************************/

CMLink*
CMGetLink()
{
	assert( theLink != NULL );
	return theLink;
}

/******************************************************************************
 GetCommandDirector

 ******************************************************************************/

CMCommandDirector*
CMGetCommandDirector()
{
	assert( theCmdDir != NULL );
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
