/******************************************************************************
 SCCircuitDocument.cpp

	Class to display a netlist and perform analysis.

	BASE CLASS = JXFileDocument

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCCircuitDocument.h"
#include "SCVarListDirector.h"
#include "SCXferFnDirector.h"
#include "SCNDIDirector.h"
#include "SCZinDirector.h"
#include "SCEETDirector.h"
#include "SCFeedbackDirector.h"
#include "SCScratchDirector.h"
#include "SCPlotDirector.h"
#include "SCFileVersions.h"
#include "scHelpText.h"
#include "scGlobals.h"

#include "SCCircuit.h"
#include "SCCircuitVarList.h"
#include "SCComponent.h"

#include <JXDocumentManager.h>
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXStaticText.h>

#include <JFunction.h>
#include <JString.h>
#include <jCommandLine.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JCharacter* kSymcirFileSuffix = ".jsc";

static const JCharacter* kDataFileSignature = "jx_symcir_notebook";
const JSize kDataFileSignatureLength        = strlen(kDataFileSignature);

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"Open... %k Meta-O | Save %k Meta-S | Save as... | Revert to saved"
	"%l| Quit %k Meta-Q";

enum
{
	kOpenSomethingCmd = 1, kSaveCmd, kSaveAsCmd, kRevertCmd,
	kQuitCmd
};

// Analysis menu

static const JCharacter* kAnalysisMenuTitleStr = "Analysis";
static const JCharacter* kAnalysisMenuStr =
	"Show variables %k Meta-V"
	"%l| Transfer function %k Meta-T | Null-double injection %k Meta-D"
	"| Input impedance %k Meta-I"
	"| Extra Element Theorem %k Meta-E | Feedback analysis %k Meta-F"
	"| Scratch window";

enum
{
	kShowVarListCmd = 1,
	kXFerFnCmd,
	kNDICmd,
	kZinCmd,
	kEETCmd,
	kFeedbackCmd,
	kScratchCmd
};

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"About"
	"%l| Notes | Expressions"
	"%l| Changes | Credits";

enum
{
	kAboutCmd = 1,
	kMiscHelpCmd, kExprHelpCmd,
	kChangeLogCmd, kCreditsCmd,

	kFirstHelpCmd = kMiscHelpCmd	// same order as in SCHelpText.cc
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SCCircuitDocument::SCCircuitDocument
	(
	JXDirector*			supervisor,
	const JCharacter*	fileName
	)
	:
	JXFileDocument(supervisor, "", kJFalse, kJTrue, kSymcirFileSuffix)
{
	itsCircuit = NULL;
	itsVarList = NULL;

	itsExprList = new JPtrArray<SCDirectorBase>(JPtrArrayT::kForgetAll);
	assert( itsExprList != NULL );

	itsPlotList = new JPtrArray<SCPlotDirector>(JPtrArrayT::kForgetAll);
	assert( itsPlotList != NULL );
	itsPlotList->SetCompareFunction(ComparePlotDirectorTitles);
	itsPlotList->SetSortOrder(JOrderedSetT::kSortAscending);

	itsIgnoreNewDirectorFlag = kJFalse;

	BuildWindow();

	itsVarListDir = new SCVarListDirector(this);
	assert( itsVarListDir != NULL );

	OpenSomething(fileName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCCircuitDocument::~SCCircuitDocument()
{
	delete itsExprList;
	delete itsPlotList;
	delete itsVarList;
	delete itsCircuit;
}

/******************************************************************************
 BuildWindow (private)

	This must not rely on having a Circuit or a VariableList.

 ******************************************************************************/

void
SCCircuitDocument::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 260,300, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 260,30);
	assert( menuBar != NULL );

	itsNetlistText =
		new JXStaticText("", kJFalse, kJTrue, NULL, JGetString("itsNetlistText::SCCircuitDocument::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 260,270);
	assert( itsNetlistText != NULL );

// end JXLayout

	window->SetCloseAction(JXWindow::kQuitApp);
	window->SetMinSize(100,100);

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsAnalysisMenu = menuBar->AppendTextMenu(kAnalysisMenuTitleStr);
	itsAnalysisMenu->SetMenuItems(kAnalysisMenuStr);
	itsAnalysisMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsAnalysisMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCCircuitDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateHelpMenu();
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsAnalysisMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateAnalysisMenu();
		}
	else if (sender == itsAnalysisMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleAnalysisMenu(selection->GetIndex());
		}

	else if (sender == itsVarList &&
			 (message.Is(JVariableList::kVarInserted) ||
			  message.Is(JVariableList::kVarNameChanged) ||
			  message.Is(JVariableList::kVarValueChanged)))
		{
		DataModified();
		}

	else
		{
		JXFileDocument::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
SCCircuitDocument::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
SCCircuitDocument::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		(SCGetApplication())->DisplayAbout(GetDisplay());
		}
	else
		{
		(JXGetHelpManager())->
			ShowSection( kSCHelpSectionName [ index - kFirstHelpCmd ] );
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
SCCircuitDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kSaveCmd, NeedsSave());
	itsFileMenu->SetItemEnable(kRevertCmd, CanRevert());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
SCCircuitDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenSomethingCmd)
		{
		OpenSomething();
		}

	else if (index == kSaveCmd)
		{
		Save();
		}
	else if (index == kSaveAsCmd)
		{
		SaveInNewFile();
		}
	else if (index == kRevertCmd)
		{
		RevertToSaved();
		}

	else if (index == kQuitCmd)
		{
		(SCGetApplication())->Quit();
		}
}

/******************************************************************************
 OpenSomething

 ******************************************************************************/

void
SCCircuitDocument::OpenSomething
	(
	const JCharacter* fileName
	)
{
	JString fullName;
	if (fileName != NULL && fileName[0] != '\0')
		{
		if (JFileExists(fileName))
			{
			fullName = fileName;
			}
		else
			{
			return;
			}
		}
	else if (!(JGetChooseSaveFile())->ChooseFile("File to open:", NULL, &fullName))
		{
		return;
		}

	std::ifstream input(fullName);
	const FileStatus status = CanReadFile(input);
	JXFileDocument* doc;
	if (status == kFileReadable &&
		!(JXGetDocumentManager())->FileDocumentIsOpen(fullName, &doc) &&
		OKToClose())
		{
		CloseAllWindows();
		FileChanged(fullName, kJTrue);
		ReadFile(input);
		}
	else if (status == kNeedNewerVersion)
		{
		(JGetUserNotification())->ReportError(
			"This notebook was created by a newer version of Symcir.  "
			"You need the newest version in order to open it.");
		}
	else if (status == kNotMyFile && OKToClose())
		{
		input.close();
		CloseAllWindows();
		ReadNetlist(fullName);
		}
}

/******************************************************************************
 RevertToSaved (private)

 ******************************************************************************/

void
SCCircuitDocument::RevertToSaved()
{
	if (OKToRevert())
		{
		DiscardChanges();
		}
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
SCCircuitDocument::DiscardChanges()
{
	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
		{
		CloseAllWindows();
		std::ifstream input(fullName);
		ReadFile(input);
		}
	else
		{
		assert( 0 /* "New file" not supported! */ );
		}

	DataReverted();
}

/******************************************************************************
 ReadFile (private)

 ******************************************************************************/

void
SCCircuitDocument::ReadFile
	(
	std::istream& input
	)
{
	assert( itsExprList->IsEmpty() && itsPlotList->IsEmpty() );

	input.ignore(kDataFileSignatureLength);

	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentMainFileVersion );

	JString netlistText;
	input >> netlistText;
	itsNetlistText->SetText(netlistText);

	delete itsVarList;
	delete itsCircuit;

	itsCircuit = new SCCircuit(input, vers);
	assert( itsCircuit != NULL );

	itsVarList = new SCCircuitVarList(input, vers);
	assert( itsVarList != NULL );

	ListenTo(itsVarList);
	itsVarListDir->SetVarList(itsVarList);

	GetWindow()->ReadGeometry(input);
	itsVarListDir->ReadSetup(input, vers);

	itsIgnoreNewDirectorFlag = kJTrue;

	JSize exprCount;
	input >> exprCount;

	for (JIndex i=1; i<=exprCount; i++)
		{
		SCDirectorBase* dir = SCDirectorBase::StreamIn(input, vers, this);
		dir->Activate();
		}

	if (vers >= 1)
		{
		JSize plotCount;
		input >> plotCount;

		for (JIndex i=1; i<=plotCount; i++)
			{
			SCPlotDirector* dir = new SCPlotDirector(input, vers, this);
			dir->Activate();
			}
		}

	itsIgnoreNewDirectorFlag = kJFalse;
}

/******************************************************************************
 WriteTextFile (virtual protected)

 ******************************************************************************/

void
SCCircuitDocument::WriteTextFile
	(
	std::ostream&		output,
	const JBoolean	safetySave
	)
	const
{
JIndex i;

	output << kDataFileSignature;
	output << ' ' << kCurrentMainFileVersion;

	output << ' ' << itsNetlistText->GetText();

	output << ' ' << *itsCircuit;
	output << ' ' << *itsVarList;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsVarListDir->WriteSetup(output);

	const JSize exprCount = itsExprList->GetElementCount();
	output << ' ' << exprCount;

	for (i=1; i<=exprCount; i++)
		{
		output << ' ';
		(itsExprList->NthElement(i))->StreamOut(output);
		}

	const JSize plotCount = itsPlotList->GetElementCount();
	output << ' ' << plotCount;

	for (i=1; i<=plotCount; i++)
		{
		output << ' ';
		(itsPlotList->NthElement(i))->StreamOut(output);
		}
}

/******************************************************************************
 CanReadFile (static)

 ******************************************************************************/

JXFileDocument::FileStatus
SCCircuitDocument::CanReadFile
	(
	std::istream& input
	)
{
	JFileVersion actualFileVersion;
	return DefaultCanReadASCIIFile(input, kDataFileSignature, kCurrentMainFileVersion,
								   &actualFileVersion);
}

/******************************************************************************
 ReadNetlist (private)

 ******************************************************************************/

void
SCCircuitDocument::ReadNetlist
	(
	const JCharacter* fileName
	)
{
	JString path, trueFileName;
	JSplitPathAndName(fileName, &path, &trueFileName);
	FileChanged(trueFileName, kJFalse);

	delete itsVarList;
	delete itsCircuit;

	itsCircuit = new SCCircuit(fileName);
	assert( itsCircuit != NULL );

	itsVarList = new SCCircuitVarList;
	assert( itsVarList != NULL );
	itsCircuit->BuildVariableList(itsVarList);

	ListenTo(itsVarList);
	itsVarListDir->SetVarList(itsVarList);

	JString text;
	JReadFile(fileName, &text);
	itsNetlistText->SetText(text);
}

/******************************************************************************
 UpdateAnalysisMenu (private)

 ******************************************************************************/

void
SCCircuitDocument::UpdateAnalysisMenu()
{
}

/******************************************************************************
 HandleAnalysisMenu (private)

 ******************************************************************************/

void
SCCircuitDocument::HandleAnalysisMenu
	(
	const JIndex index
	)
{
	if (index == kShowVarListCmd)
		{
		itsVarListDir->Activate();
		}
	else if (index == kXFerFnCmd)
		{
		SCXferFnDirector* dir = new SCXferFnDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
	else if (index == kNDICmd)
		{
		SCNDIDirector* dir = new SCNDIDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
	else if (index == kZinCmd)
		{
		SCZinDirector* dir = new SCZinDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
	else if (index == kEETCmd)
		{
		SCEETDirector* dir = new SCEETDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
	else if (index == kFeedbackCmd)
		{
		SCFeedbackDirector* dir = new SCFeedbackDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
	else if (index == kScratchCmd)
		{
		SCScratchDirector* dir = new SCScratchDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

	Listen for windows that are closed.

 ******************************************************************************/

void
SCCircuitDocument::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	JIndex dirIndex;
	SCDirectorBase* exprDir = (SCDirectorBase*) theDirector;
	if (exprDir != NULL && itsExprList->Find(exprDir, &dirIndex))
		{
		itsExprList->RemoveElement(dirIndex);
		DataModified();
		}
	SCPlotDirector* plotDir = (SCPlotDirector*) theDirector;
	if (plotDir != NULL && itsPlotList->Find(plotDir, &dirIndex))
		{
		itsPlotList->RemoveElement(dirIndex);
		DataModified();
		}

	JXWindowDirector::DirectorClosed(theDirector);
}

/*****************************************************************************
 CloseAllWindows (private)

 ******************************************************************************/

void
SCCircuitDocument::CloseAllWindows()
{
	while (!itsExprList->IsEmpty())
		{
		const JBoolean ok = (itsExprList->LastElement())->Close();
		assert( ok );
		}

	while (!itsPlotList->IsEmpty())
		{
		const JBoolean ok = (itsPlotList->LastElement())->Close();
		assert( ok );
		}
}

/*****************************************************************************
 BuildPlotMenu

	We are deactivated until a plot is chosen, so sorting itsPlotList
	is safe.

 ******************************************************************************/

void
SCCircuitDocument::BuildPlotMenu
	(
	JXTextMenu* menu
	)
	const
{
	itsPlotList->Sort();

	const JSize count = itsPlotList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		SCPlotDirector* dir = itsPlotList->NthElement(i);
		menu->AppendItem((dir->GetWindow())->GetTitle(), kJTrue, kJTrue);
		}

	menu->SetUpdateAction(JXMenu::kDisableNone);
}

/******************************************************************************
 ComparePlotDirectorTitles (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
SCCircuitDocument::ComparePlotDirectorTitles
	(
	SCPlotDirector* const & p1,
	SCPlotDirector* const & p2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&((p1->GetWindow())->GetTitle())),
			const_cast<JString*>(&((p2->GetWindow())->GetTitle())));
}
