/******************************************************************************
 THXApp.cpp

	BASE CLASS = JXApplication

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <thxStdInc.h>
#include "THXApp.h"
#include "THXVarList.h"
#include "THXVarDirector.h"
#include "THXExprDirector.h"
#include "THX2DPlotDirector.h"
#include "THX2DPlotFunctionDialog.h"
#include "THX3DPlotDirector.h"
#include "THX3DPlotFunctionDialog.h"
#include "THXBaseConvDirector.h"
#include "THXAboutDialog.h"
#include "thxGlobals.h"
#include "thxStringData.h"
#include "thxFileVersions.h"
#include "thxHelpText.h"
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXPSPrinter.h>
#include <JX2DPlotEPSPrinter.h>
#include <JXChooseSaveFile.h>
#include <JXSharedPrefsManager.h>
#include <JXSplashWindow.h>
#include <jXActionDefs.h>
#include <JThisProcess.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <fstream>
#include <sstream>
#include <jAssert.h>

#include <new_planet_software.xpm>

// application signature (MDI)

static const JCharacter* kAppSignature = "leibnitz";

// state information

static const JCharacter* kStateFileName    = ".leibnitz.session";
static const JCharacter* kOldStateFileName = ".THX-1138.session";

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About" 
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kHelpAboutCmd = 1,
	kHelpTOCCmd, kHelpOverviewCmd, kHelpWindowCmd,
	kHelpChangeLogCmd, kHelpCreditsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

THXApp::THXApp
	(
	int*	argc,
	char*	argv[]
	)
	:
	JXApplication(argc, argv, kAppSignature, kTHXDefaultStringData)
{
	itsStartupFlag = kJTrue;

	itsExprList = new JPtrArray<THXExprDirector>(JPtrArrayT::kForgetAll);
	assert( itsExprList != NULL );

	itsKeyPadVisibleFlag = kJTrue;

	its2DPlotList = new JPtrArray<THX2DPlotDirector>(JPtrArrayT::kForgetAll);
	assert( its2DPlotList != NULL );
	its2DPlotList->SetCompareFunction(Compare2DPlotTitles);
	its2DPlotList->SetSortOrder(JOrderedSetT::kSortAscending);

	its3DPlotList = new JPtrArray<THX3DPlotDirector>(JPtrArrayT::kForgetAll);
	assert( its3DPlotList != NULL );
	its3DPlotList->SetCompareFunction(Compare3DPlotTitles);
	its3DPlotList->SetSortOrder(JOrderedSetT::kSortAscending);

	its2DPlotFnDialog = NULL;
	its3DPlotFnDialog = NULL;

	THXCreateGlobals(this);
	RestoreProgramState();

	itsStartupFlag = kJFalse;
}

/******************************************************************************
 Destructor

	Delete global objects in reverse order of creation.

 ******************************************************************************/

THXApp::~THXApp()
{
	delete itsVarList;
	delete itsExprList;		// objects deleted by JXDirector
	delete its2DPlotList;	// objects deleted by JXDirector
	delete its3DPlotList;	// objects deleted by JXDirector

	THXDeleteGlobals();
}

/******************************************************************************
 Close (virtual protected)

 ******************************************************************************/

JBoolean
THXApp::Close()
{
	if (!itsVarDirector->OKToDeactivate())
		{
		return kJFalse;
		}

	SaveProgramState();
	return JXApplication::Close();
}

/******************************************************************************
 DisplayAbout

 ******************************************************************************/

void
THXApp::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	THXAboutDialog* dlog = new THXAboutDialog(this, prevVersStr);
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 NewExpression

 ******************************************************************************/

THXExprDirector*
THXApp::NewExpression
	(
	const JBoolean centerOnScreen
	)
{
	THXExprDirector* expr = new THXExprDirector(this, itsVarList);
	assert( expr != NULL );
	if (centerOnScreen)
		{
		(expr->GetWindow())->PlaceAsDialogWindow();
		}
	expr->Activate();
	itsExprList->Append(expr);
	return expr;
}

/******************************************************************************
 SetKeyPadVisible

 ******************************************************************************/

void
THXApp::SetKeyPadVisible
	(
	const JBoolean visible
	)
{
	if (visible != itsKeyPadVisibleFlag)
		{
		itsKeyPadVisibleFlag = visible;

		const JSize count = itsExprList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsExprList->NthElement(i))->UpdateDisplay();
			}
		}
}

/******************************************************************************
 New2DPlot

	prevPlot should be the THX2DPlotDirector that originated the request,
	NULL otherwise.

 ******************************************************************************/

void
THXApp::New2DPlot
	(
	const THX2DPlotDirector* prevPlot
	)
{
	assert( its2DPlotFnDialog == NULL );

	its2DPlotFnDialog = new THX2DPlotFunctionDialog(this, itsVarList, prevPlot);
	assert( its2DPlotFnDialog != NULL );
	its2DPlotFnDialog->BeginDialog();
	ListenTo(its2DPlotFnDialog);
}

/******************************************************************************
 Create2DPlot (private)

 ******************************************************************************/

void
THXApp::Create2DPlot()
{
	assert( its2DPlotFnDialog != NULL );

	JIndex plotIndex;
	const JFunction* f;
	JString curveName;
	JFloat xMin, xMax;
	its2DPlotFnDialog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

	if (plotIndex > its2DPlotList->GetElementCount())
		{
		THX2DPlotDirector* plot = new THX2DPlotDirector(this);
		assert( plot != NULL );
		its2DPlotList->Append(plot);
		}

	THX2DPlotDirector* plot = its2DPlotList->NthElement(plotIndex);
	plot->AddFunction(itsVarList, *f, curveName, xMin, xMax);
	plot->Activate();
}

/******************************************************************************
 New3DPlot

	prevPlot should be the THX3DPlotDirector that originated the request,
	NULL otherwise.

 ******************************************************************************/

void
THXApp::New3DPlot
	(
	const THX3DPlotDirector* prevPlot
	)
{
	assert( its3DPlotFnDialog == NULL );

	its3DPlotFnDialog = new THX3DPlotFunctionDialog(this, itsVarList, prevPlot);
	assert( its3DPlotFnDialog != NULL );
	its3DPlotFnDialog->BeginDialog();
	ListenTo(its3DPlotFnDialog);
}

/******************************************************************************
 Create3DPlot (private)

 ******************************************************************************/

void
THXApp::Create3DPlot()
{
	assert( its3DPlotFnDialog != NULL );

	const JFunction* f;
	JString surfaceName;
	JFloat xMin, xMax, yMin, yMax;
	JSize xCount, yCount;
	its3DPlotFnDialog->GetSettings(&f, &surfaceName, &xMin, &xMax, &xCount,
								   &yMin, &yMax, &yCount);

	THX3DPlotDirector* plot = new THX3DPlotDirector(this);
	assert( plot != NULL );
	its3DPlotList->Append(plot);

	plot->SetFunction(itsVarList, *f, surfaceName,
					  xMin, xMax, xCount, yMin, yMax, yCount);
	plot->Activate();
}

/******************************************************************************
 ShowConstants

 ******************************************************************************/

void
THXApp::ShowConstants()
	const
{
	itsVarDirector->Activate();
}

/******************************************************************************
 ShowBaseConversion

 ******************************************************************************/

void
THXApp::ShowBaseConversion()
	const
{
	itsBCDirector->Activate();
}

/******************************************************************************
 RestoreProgramState (private)

 ******************************************************************************/

void
THXApp::RestoreProgramState()
{
JIndex i;

	if (!JGetPrefsDirectory(&itsStatePath))
		{
		(JGetUserNotification())->ReportError(
			"I am unable to find your preferences directory.");
		JThisProcess::Exit(1);
		}

	const JString fullName = JCombinePathAndName(itsStatePath, kStateFileName);
	if (!JFileExists(fullName))
		{
		const JString oldName = JCombinePathAndName(itsStatePath, kOldStateFileName);
		if (!JFileExists(oldName) || !(JRenameFile(oldName, fullName)).OK())
			{
			InitProgramState();
			DisplayAbout();
			return;
			}
		}

	ifstream input(fullName);

	JFileVersion vers;
	input >> vers;
	if (input.eof() || input.fail())
		{
		InitProgramState();
		return;
		}
	else if (vers > kCurrentStateVersion)
		{
		JString msg = "Unable to read the file \"";
		msg += fullName;
		msg += "\" because it was created by a newer version of this program.";
		(JGetUserNotification())->ReportError(msg);
		JThisProcess::Exit(1);
		}

	JString prevProgramVers;
	input >> prevProgramVers;

	JBoolean displayAbout = kJFalse;
	if (prevProgramVers != THXGetVersionNumberStr())
		{
		if (!(JGetUserNotification())->AcceptLicense())
			{
			JThisProcess::Exit(0);
			}

		displayAbout = kJTrue;
		}

	itsVarList = new THXVarList(input, vers);
	assert( itsVarList != NULL );

	itsVarDirector = new THXVarDirector(input, vers, this, itsVarList);
	assert( itsVarDirector != NULL );

	itsBCDirector = new THXBaseConvDirector(input, vers, this);
	assert( itsBCDirector != NULL );

	if (vers >= 1)
		{
		input >> itsKeyPadVisibleFlag;
		}

	if (vers >= 2)
		{
		THXExprDirector::ReadPrefs(input, vers);

		if (vers < 8 && (JXGetSharedPrefsManager())->WasNew())
			{
			(JXGetHelpManager())->ReadPrefs(input);
			}
		else if (vers < 8)
			{
			std::ostringstream data;
			(JXGetHelpManager())->WritePrefs(data, (JXGetHelpManager())->GetCurrentPrefsVersion());

			(JXGetHelpManager())->ReadPrefs(input);

			const std::string s = data.str();
			std::istringstream in2(s);
			(JXGetHelpManager())->ReadPrefs(in2);
			(JXGetHelpManager())->JXSharedPrefObject::WritePrefs();
			}
		}

	if (vers >= 5)
		{
		(JXGetChooseSaveFile())->ReadSetup(input);
		}

	if (vers >= 3)
		{
		(THXGetPSGraphPrinter())->ReadXPSSetup(input);
		}
	if (vers >= 7)
		{
		(THXGetEPSGraphPrinter())->ReadX2DEPSSetup(input);
		}

	JSize exprCount;
	input >> exprCount;

	if (exprCount == 0)
		{
		NewExpression(kJTrue);
		}
	else
		{
		for (i=1; i<=exprCount; i++)
			{
			THXExprDirector* expr = new THXExprDirector(input, vers, this, itsVarList);
			assert( expr != NULL );
			expr->Activate();
			itsExprList->Append(expr);

			if (exprCount == 1)
				{
				(expr->GetWindow())->Deiconify();
				}
			}
		}

	JSize plotCount;
	input >> plotCount;

	for (i=1; i<=plotCount; i++)
		{
		THX2DPlotDirector* plot = new THX2DPlotDirector(input, vers, this, itsVarList);
		assert( plot != NULL );
		plot->Activate();
		its2DPlotList->Append(plot);
		}

	if (vers >= 10)
		{
		plotCount;
		input >> plotCount;

		for (i=1; i<=plotCount; i++)
			{
			THX3DPlotDirector* plot = new THX3DPlotDirector(input, vers, this, itsVarList);
			assert( plot != NULL );
			plot->Activate();
			its3DPlotList->Append(plot);
			}
		}

	if (displayAbout)
		{
		DisplayAbout(prevProgramVers);
		}
#ifdef DISPLAY_SPLASH
	else
		{
		JXSplashWindow* w = new JXSplashWindow(new_planet_software, THXGetVersionStr(),
											   SPLASH_DISPLAY_TIME);
		assert( w != NULL );
		w->Activate();
		}
#endif
}

/******************************************************************************
 InitProgramState (private)

 ******************************************************************************/

void
THXApp::InitProgramState()
{
	if (!(JGetUserNotification())->AcceptLicense())
		{
		JThisProcess::Exit(0);
		}

	itsVarList = new THXVarList;
	assert( itsVarList != NULL );

	itsVarDirector = new THXVarDirector(this, itsVarList);
	assert( itsVarDirector != NULL );

	itsBCDirector = new THXBaseConvDirector(this);
	assert( itsBCDirector != NULL );

	NewExpression(kJTrue);
}

/******************************************************************************
 SaveProgramState (private)

 ******************************************************************************/

void
THXApp::SaveProgramState()
{
JIndex i;

	const JString fullName = JCombinePathAndName(itsStatePath, kStateFileName);
	ofstream output(fullName);

	output << kCurrentStateVersion;
	output << ' ' << JString(THXGetVersionNumberStr());
	output << ' ' << *itsVarList;

	output << ' ';
	itsVarDirector->WriteState(output);

	output << ' ';
	itsBCDirector->WriteState(output);

	output << ' ' << itsKeyPadVisibleFlag;

	output << ' ';
	THXExprDirector::WritePrefs(output);

	output << ' ';
	(JXGetChooseSaveFile())->WriteSetup(output);

	output << ' ';
	(THXGetPSGraphPrinter())->WriteXPSSetup(output);

	output << ' ';
	(THXGetEPSGraphPrinter())->WriteX2DEPSSetup(output);

	const JSize exprCount = itsExprList->GetElementCount();
	output << ' ' << exprCount;

	for (i=1; i<=exprCount; i++)
		{
		output << ' ';
		(itsExprList->NthElement(i))->WriteState(output);
		}

	JSize plotCount = its2DPlotList->GetElementCount();
	output << ' ' << plotCount;

	for (i=1; i<=plotCount; i++)
		{
		output << ' ';
		(its2DPlotList->NthElement(i))->WriteState(output);
		}

	plotCount = its3DPlotList->GetElementCount();
	output << ' ' << plotCount;

	for (i=1; i<=plotCount; i++)
		{
		output << ' ';
		(its3DPlotList->NthElement(i))->WriteState(output);
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
THXApp::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == its2DPlotFnDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			Create2DPlot();
			}
		its2DPlotFnDialog = NULL;
		}

	else if (sender == its3DPlotFnDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			Create3DPlot();
			}
		its3DPlotFnDialog = NULL;
		}

	else
		{
		JXApplication::Receive(sender, message);
		}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

	Listen for windows that are closed.

	You cannot use dynamic_cast because the object is already at least
	partially deleted.

 ******************************************************************************/

void
THXApp::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	JIndex dirIndex;
	THXExprDirector* exprDir = (THXExprDirector*) theDirector;
	if (exprDir != NULL && itsExprList->Find(exprDir, &dirIndex))
		{
		itsExprList->RemoveElement(dirIndex);
		}
	THX2DPlotDirector* plot2DDir = (THX2DPlotDirector*) theDirector;
	if (plot2DDir != NULL && its2DPlotList->Find(plot2DDir, &dirIndex))
		{
		its2DPlotList->RemoveElement(dirIndex);
		}
	THX3DPlotDirector* plot3DDir = (THX3DPlotDirector*) theDirector;
	if (plot3DDir != NULL && its3DPlotList->Find(plot3DDir, &dirIndex))
		{
		its3DPlotList->RemoveElement(dirIndex);
		}

	JXApplication::DirectorClosed(theDirector);

	if (!itsStartupFlag && itsExprList->IsEmpty())
		{
		Quit();
		}
}

/*****************************************************************************
 BuildPlotMenu

	If origPlot != NULL, initialChoice is set to the index of the director.

	We are deactivated until a plot is chosen, so sorting its2DPlotList
	is safe.

 ******************************************************************************/

void
THXApp::BuildPlotMenu
	(
	JXTextMenu*					menu,
	const THX2DPlotDirector*	origPlot,
	JIndex*						initialChoice
	)
	const
{
	its2DPlotList->Sort();

	*initialChoice = 0;

	const JSize count = its2DPlotList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const THX2DPlotDirector* plot = its2DPlotList->NthElement(i);
		menu->AppendItem((plot->GetWindow())->GetTitle(), JXMenu::kRadioType);

		if (plot == origPlot)
			{
			*initialChoice = i;
			}
		}

	if (*initialChoice == 0)
		{
		*initialChoice = count+1;
		}

	if (count > 0)
		{
		menu->ShowSeparatorAfter(count);
		}
	menu->AppendItem("New window", JXMenu::kRadioType);
	menu->SetUpdateAction(JXMenu::kDisableNone);
}

/******************************************************************************
 Compare2DPlotTitles (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
THXApp::Compare2DPlotTitles
	(
	THX2DPlotDirector* const & p1,
	THX2DPlotDirector* const & p2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&((p1->GetWindow())->GetTitle())),
			const_cast<JString*>(&((p2->GetWindow())->GetTitle())));
}

/******************************************************************************
 Compare3DPlotTitles (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
THXApp::Compare3DPlotTitles
	(
	THX3DPlotDirector* const & p1,
	THX3DPlotDirector* const & p2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&((p1->GetWindow())->GetTitle())),
			const_cast<JString*>(&((p2->GetWindow())->GetTitle())));
}

/******************************************************************************
 CreateHelpMenu

 ******************************************************************************/

#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

JXTextMenu*
THXApp::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JCharacter*	idNamespace
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	menu->SetMenuItems(kHelpMenuStr, idNamespace);
	menu->SetUpdateAction(JXMenu::kDisableNone);

	menu->SetItemImage(kHelpTOCCmd,    jx_help_toc);
	menu->SetItemImage(kHelpWindowCmd, jx_help_specific);

	return menu;
}

/******************************************************************************
 UpdateHelpMenu

 ******************************************************************************/

void
THXApp::UpdateHelpMenu
	(
	JXTextMenu* menu
	)
{
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
THXApp::HandleHelpMenu
	(
	JXTextMenu*			menu,
	const JCharacter*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
		{
		DisplayAbout();
		}

	else if (index == kHelpTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kTHXTOCHelpName);
		}
	else if (index == kHelpOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kTHXOverviewHelpName);
		}
	else if (index == kHelpWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(windowSectionName);
		}

	else if (index == kHelpChangeLogCmd)
		{
		(JXGetHelpManager())->ShowSection(kTHXChangeLogName);
		}
	else if (index == kHelpCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kTHXCreditsName);
		}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
THXApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
		SaveProgramState();
		}
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
THXApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
THXApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kTHXDefaultStringData);
}
