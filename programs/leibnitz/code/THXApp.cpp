/******************************************************************************
 THXApp.cpp

	BASE CLASS = JXApplication

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THXApp.h"
#include "THXVarList.h"
#include "THXVarDirector.h"
#include "THXExprDirector.h"
#include "THX2DPlotDirector.h"
#include "THX2DPlotFunctionDialog.h"
#include "THXBaseConvDirector.h"
#include "THXAboutDialog.h"
#include "thxGlobals.h"
#include "thxStringData.h"
#include "thxFileVersions.h"
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXPSPrinter.h>
#include <JX2DPlotEPSPrinter.h>
#include <JXChooseSaveFile.h>
#include <JXSharedPrefsManager.h>
#include <JXSplashWindow.h>
#include <JXTipOfTheDayDialog.h>
#include <jXActionDefs.h>
#include <JThisProcess.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <fstream>
#include <sstream>
#include <jAssert.h>

// application signature (MDI)

static const JUtf8Byte* kAppSignature = "leibnitz";

// state information

static const JString kStateFileName(".leibnitz.session");
static const JString kOldStateFileName(".THX-1138.session");

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About" 
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Tip of the Day"
	"%l| Changes"
	"  | Credits";

enum
{
	kHelpAboutCmd = 1,
	kHelpTOCCmd, kHelpOverviewCmd, kHelpWindowCmd,
	kTipCmd,
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
	itsStartupFlag = true;

	itsExprList = jnew JPtrArray<THXExprDirector>(JPtrArrayT::kForgetAll);
	assert( itsExprList != nullptr );

	itsKeyPadVisibleFlag = true;

	its2DPlotList = jnew JPtrArray<THX2DPlotDirector>(JPtrArrayT::kForgetAll);
	assert( its2DPlotList != nullptr );
	its2DPlotList->SetCompareFunction(Compare2DPlotTitles);
	its2DPlotList->SetSortOrder(JListT::kSortAscending);

	its2DPlotFnDialog = nullptr;

	THXCreateGlobals(this);
	RestoreProgramState();

	itsStartupFlag = false;
}

/******************************************************************************
 Destructor

	Delete global objects in reverse order of creation.

 ******************************************************************************/

THXApp::~THXApp()
{
	jdelete itsVarList;
	jdelete itsExprList;		// objects deleted by JXDirector
	jdelete its2DPlotList;	// objects deleted by JXDirector

	THXDeleteGlobals();
}

/******************************************************************************
 Close (virtual protected)

 ******************************************************************************/

bool
THXApp::Close()
{
	if (!itsVarDirector->OKToDeactivate())
		{
		return false;
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
	const JString& prevVersStr
	)
{
	auto* dlog = jnew THXAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 NewExpression

 ******************************************************************************/

THXExprDirector*
THXApp::NewExpression
	(
	const bool centerOnScreen
	)
{
	auto* expr = jnew THXExprDirector(this, itsVarList);
	assert( expr != nullptr );
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
	const bool visible
	)
{
	if (visible != itsKeyPadVisibleFlag)
		{
		itsKeyPadVisibleFlag = visible;

		const JSize count = itsExprList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsExprList->GetElement(i))->UpdateDisplay();
			}
		}
}

/******************************************************************************
 New2DPlot

	prevPlot should be the THX2DPlotDirector that originated the request,
	nullptr otherwise.

 ******************************************************************************/

void
THXApp::New2DPlot
	(
	const THX2DPlotDirector* prevPlot
	)
{
	assert( its2DPlotFnDialog == nullptr );

	its2DPlotFnDialog = jnew THX2DPlotFunctionDialog(this, itsVarList, prevPlot);
	assert( its2DPlotFnDialog != nullptr );
	its2DPlotFnDialog->BeginDialog();
	ListenTo(its2DPlotFnDialog);
}

/******************************************************************************
 Create2DPlot (private)

 ******************************************************************************/

void
THXApp::Create2DPlot()
{
	assert( its2DPlotFnDialog != nullptr );

	JIndex plotIndex;
	const JFunction* f;
	JString curveName;
	JFloat xMin, xMax;
	its2DPlotFnDialog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

	if (plotIndex > its2DPlotList->GetElementCount())
		{
		auto* plot = jnew THX2DPlotDirector(this);
		assert( plot != nullptr );
		its2DPlotList->Append(plot);
		}

	THX2DPlotDirector* plot = its2DPlotList->GetElement(plotIndex);
	plot->AddFunction(itsVarList, *f, curveName, xMin, xMax);
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
		JGetUserNotification()->ReportError(JGetString("NoPrefsDir::THXApp"));
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

	std::ifstream input(fullName.GetBytes());

	JFileVersion vers;
	input >> vers;
	if (input.eof() || input.fail())
		{
		InitProgramState();
		return;
		}
	else if (vers > kCurrentStateVersion)
		{
		const JUtf8Byte* map[] =
		{
			"name", fullName.GetBytes()
		};
		const JString msg = JGetString("CannotReadNewerVersion::THXApp", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		JThisProcess::Exit(1);
		}

	JString prevProgramVers;
	input >> prevProgramVers;

	bool displayAbout = false;
	if (prevProgramVers != THXGetVersionNumberStr())
		{
		if (!JGetUserNotification()->AcceptLicense())
			{
			JThisProcess::Exit(0);
			}

		displayAbout = true;
		}

	itsVarList = jnew THXVarList(input, vers);
	assert( itsVarList != nullptr );

	itsVarDirector = jnew THXVarDirector(input, vers, this, itsVarList);
	assert( itsVarDirector != nullptr );

	itsBCDirector = jnew THXBaseConvDirector(input, vers, this);
	assert( itsBCDirector != nullptr );

	if (vers >= 1)
		{
		input >> JBoolFromString(itsKeyPadVisibleFlag);
		}

	if (vers >= 2)
		{
		THXExprDirector::ReadPrefs(input, vers);

		// ignoring obsolete JXGetHelpManager data, since it was removed in version 8
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
		NewExpression(true);
		}
	else
		{
		for (i=1; i<=exprCount; i++)
			{
			auto* expr = jnew THXExprDirector(input, vers, this, itsVarList);
			assert( expr != nullptr );
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
		auto* plot = jnew THX2DPlotDirector(input, vers, this, itsVarList);
		assert( plot != nullptr );
		plot->Activate();
		its2DPlotList->Append(plot);
		}

	if (displayAbout)
		{
		DisplayAbout(prevProgramVers);
		}
}

/******************************************************************************
 InitProgramState (private)

 ******************************************************************************/

void
THXApp::InitProgramState()
{
	if (!JGetUserNotification()->AcceptLicense())
		{
		JThisProcess::Exit(0);
		}

	itsVarList = jnew THXVarList;
	assert( itsVarList != nullptr );

	itsVarDirector = jnew THXVarDirector(this, itsVarList);
	assert( itsVarDirector != nullptr );

	itsBCDirector = jnew THXBaseConvDirector(this);
	assert( itsBCDirector != nullptr );

	NewExpression(true);
}

/******************************************************************************
 SaveProgramState (private)

 ******************************************************************************/

void
THXApp::SaveProgramState()
{
JIndex i;

	const JString fullName = JCombinePathAndName(itsStatePath, kStateFileName);
	std::ofstream output(fullName.GetBytes());

	output << kCurrentStateVersion;
	output << ' ' << JString(THXGetVersionNumberStr());
	output << ' ' << *itsVarList;

	output << ' ';
	itsVarDirector->WriteState(output);

	output << ' ';
	itsBCDirector->WriteState(output);

	output << ' ' << JBoolToString(itsKeyPadVisibleFlag);

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
		(itsExprList->GetElement(i))->WriteState(output);
		}

	JSize plotCount = its2DPlotList->GetElementCount();
	output << ' ' << plotCount;

	for (i=1; i<=plotCount; i++)
		{
		output << ' ';
		(its2DPlotList->GetElement(i))->WriteState(output);
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
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			Create2DPlot();
			}
		its2DPlotFnDialog = nullptr;
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
	auto* exprDir = (THXExprDirector*) theDirector;
	if (exprDir != nullptr && itsExprList->Find(exprDir, &dirIndex))
		{
		itsExprList->RemoveElement(dirIndex);
		}
	auto* plot2DDir = (THX2DPlotDirector*) theDirector;
	if (plot2DDir != nullptr && its2DPlotList->Find(plot2DDir, &dirIndex))
		{
		its2DPlotList->RemoveElement(dirIndex);
		}

	JXApplication::DirectorClosed(theDirector);

	if (!itsStartupFlag && itsExprList->IsEmpty())
		{
		Quit();
		}
}

/*****************************************************************************
 BuildPlotMenu

	If origPlot != nullptr, initialChoice is set to the index of the director.

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
		const THX2DPlotDirector* plot = its2DPlotList->GetElement(i);
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
	menu->AppendItem(JGetString("NewWindowItem::THXApp"), JXMenu::kRadioType);
	menu->SetUpdateAction(JXMenu::kDisableNone);
}

/******************************************************************************
 Compare2DPlotTitles (static private)

 ******************************************************************************/

JListT::CompareResult
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
 CreateHelpMenu

 ******************************************************************************/

#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

JXTextMenu*
THXApp::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JUtf8Byte*	idNamespace
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
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
	const JUtf8Byte*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
		{
		DisplayAbout();
		}

	else if (index == kHelpTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kHelpOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection("THXOverviewHelp");
		}
	else if (index == kHelpWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(windowSectionName);
		}

	else if (index == kTipCmd)
		{
		auto* dlog = jnew JXTipOfTheDayDialog;
		assert( dlog != nullptr );
		dlog->BeginDialog();
		}

	else if (index == kHelpChangeLogCmd)
		{
		(JXGetHelpManager())->ShowChangeLog();
		}
	else if (index == kHelpCreditsCmd)
		{
		(JXGetHelpManager())->ShowCredits();
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

const JUtf8Byte*
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
	JGetStringManager()->Register(kAppSignature, kTHXDefaultStringData);
}
