/******************************************************************************
 THX2DPlotDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THX2DPlotDirector.h"
#include "THX2DPlotFunctionDialog.h"
#include "THXVarList.h"
#include "thxGlobals.h"
#include "thxHelpText.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXHelpManager.h>
#include <JXImage.h>
#include <jXActionDefs.h>
#include <JX2DPlotWidget.h>
#include <J2DPlotJFunction.h>
#include <JFunction.h>
#include <jAssert.h>

// Actions menu

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr =
	"    New expression      %k Meta-N"
	"  | Edit constants      %k Meta-E"
	"  | Plot 2D function... %k Meta-Shift-P"
	"  | Plot 3D function... %k Meta-Shift-O"
	"  | Base conversion...  %k Meta-B"
	"%l| PostScript page setup..."
	"  | Print PostScript... %k Meta-P"
	"%l| Print plot as EPS..."
	"  | Print marks as EPS..."
	"%l| Close window        %k Meta-W"
	"  | Quit                %k Meta-Q";

enum
{
	kNewExprCmd = 1,
	kEditConstCmd,
	kNew2DPlotCmd,
	kNew3DPlotCmd,
	kConvBaseCmd,
	kPSPageSetupCmd, kPrintPSCmd,
	kPrintPlotEPSCmd, kPrintMarksEPSCmd,
	kCloseWindowCmd, kQuitCmd
};

// Curve options pop up menu

static const JCharacter* kEditFunctionItemStr = "Edit function";

/******************************************************************************
 Constructor

 ******************************************************************************/

THX2DPlotDirector::THX2DPlotDirector
	(
	JXDirector*	supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	THX2DPlotDirectorX();
	BuildWindow();
}

THX2DPlotDirector::THX2DPlotDirector
	(
	istream&			input,
	const JFileVersion	vers,
	JXDirector*			supervisor,
	THXVarList*			varList
	)
	:
	JXWindowDirector(supervisor)
{
	THX2DPlotDirectorX();

	BuildWindow();
	GetWindow()->ReadGeometry(input);

	JSize fnCount;
	input >> fnCount;

	for (JIndex i=1; i<=fnCount; i++)
		{
		JFloat xMin, xMax;
		input >> xMin >> xMax;

		JFunction* f = JFunction::StreamIn(input, varList);
		AddFunction(varList, f, "", xMin, xMax);
		}

	itsPlotWidget->PWXReadSetup(input);
	itsPlotWidget->PWReadCurveSetup(input);

	if (4 <= vers && vers <= 5)
		{
		JString s;
		input >> s;
		itsPlotWidget->SetPSPrintFileName(s);
		}
}

// private

void
THX2DPlotDirector::THX2DPlotDirectorX()
{
	itsFnList = new JPtrArray<J2DPlotJFunction>(JPtrArrayT::kForgetAll);
	assert( itsFnList != NULL );

	itsEditFnDialog = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THX2DPlotDirector::~THX2DPlotDirector()
{
	delete itsFnList;	// contents not owned
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
THX2DPlotDirector::WriteState
	(
	ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	const JSize fnCount = itsPlotWidget->GetCurveCount();
	output << ' ' << fnCount;

	for (JIndex i=1; i<=fnCount; i++)
		{
		const JPlotDataBase& data      = itsPlotWidget->GetCurve(i);
		const J2DPlotJFunction* fnData = dynamic_cast<const J2DPlotJFunction*>(&data);
		assert( fnData != NULL );

		JFloat xMin, xMax;
		fnData->GetXRange(&xMin, &xMax);
		output << ' ' << xMin << ' ' << xMax;

		output << ' ';
		(fnData->GetFunction()).StreamOut(output);
		}

	output << ' ';
	itsPlotWidget->PWXWriteSetup(output);

	output << ' ';
	itsPlotWidget->PWWriteCurveSetup(output);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_2D_plot_window.xpm"

void
THX2DPlotDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 600,400, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( menuBar != NULL );

	itsPlotWidget =
		new JX2DPlotWidget(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,370);
	assert( itsPlotWidget != NULL );

// end JXLayout

	window->SetMinSize(300, 200);
	window->SetWMClass(THXGetWMClassInstance(), THXGetPlotWindowClass());

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, thx_2D_plot_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	itsPlotWidget->SetPSPrinter(THXGetPSGraphPrinter());
	itsPlotWidget->SetEPSPrinter(THXGetEPSGraphPrinter());

	ListenTo(itsPlotWidget);

	itsActionsMenu = menuBar->PrependTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	JXTextMenu* optionsMenu               = itsPlotWidget->GetOptionsMenu();
	const JIndex editFunctionSubmenuIndex = optionsMenu->GetItemCount()+1;
	optionsMenu->AppendItem(kEditFunctionItemStr);

	itsEditFnMenu = new JXTextMenu(optionsMenu, editFunctionSubmenuIndex, menuBar);
	assert( itsEditFnMenu != NULL );
	itsEditFnMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsEditFnMenu);

	itsHelpMenu = (THXGetApplication())->CreateHelpMenu(menuBar, "THX2DPlotDirector");
	ListenTo(itsHelpMenu);

	JXTextMenu* curveOptionsMenu = itsPlotWidget->GetCurveOptionsMenu();
	itsEditFunctionItemIndex     = curveOptionsMenu->GetItemCount()+1;
	curveOptionsMenu->ShowSeparatorAfter(itsEditFunctionItemIndex-1);
	curveOptionsMenu->AppendItem(kEditFunctionItemStr);
	ListenTo(curveOptionsMenu);

	// do this after everything is constructed so Receive() doesn't crash

	itsPlotWidget->SetTitle("New plot");
}

/******************************************************************************
 AddFunction

 ******************************************************************************/

void
THX2DPlotDirector::AddFunction
	(
	THXVarList*			varList,
	const JFunction&	f,
	const JCharacter*	name,
	const JFloat		xMin,
	const JFloat		xMax
	)
{
	AddFunction(varList, f.Copy(), name, xMin, xMax);
}

/******************************************************************************
 AddFunction (private)

	We take ownership of the function.

 ******************************************************************************/

void
THX2DPlotDirector::AddFunction
	(
	THXVarList*			varList,
	JFunction*			f,
	const JCharacter*	name,
	const JFloat		xMin,
	const JFloat		xMax
	)
{
	J2DPlotJFunction* data =
		new J2DPlotJFunction(itsPlotWidget, varList, f, kJTrue,
							 THXVarList::kXIndex, xMin, xMax);
	assert( data != NULL );

	itsPlotWidget->AddCurve(data, kJTrue, name);

	itsFnList->Append(data);
	ListenTo(data);
	UpdateEditFnMenu();
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
THX2DPlotDirector::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	const JSize count = itsFnList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (sender == itsFnList->NthElement(i))
			{
			itsFnList->RemoveElement(i);
			break;
			}
		}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
THX2DPlotDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlotWidget && message.Is(J2DPlotWidget::kTitleChanged))
		{
		GetWindow()->SetTitle(itsPlotWidget->GetTitle());
		}

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsEditFnMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEditFnMenu();
		}
	else if (sender == itsEditFnMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleEditFnMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		(THXGetApplication())->UpdateHelpMenu(itsHelpMenu);
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		(THXGetApplication())->HandleHelpMenu(itsHelpMenu, kTHX2DPlotHelpName,
											  selection->GetIndex());
		}

	else if (sender == itsPlotWidget->GetCurveOptionsMenu() &&
			 message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCurveOptionsMenu();
		}
	else if (sender == itsPlotWidget->GetCurveOptionsMenu() &&
			 message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleCurveOptionsMenu(selection->GetIndex());
		}

	else if (sender == itsEditFnDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			UpdateFunction();
			}
		itsEditFnDialog = NULL;
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
THX2DPlotDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnable(kPrintMarksEPSCmd, itsPlotWidget->HasMarks());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
THX2DPlotDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kNewExprCmd)
		{
		(THXGetApplication())->NewExpression();
		}
	else if (index == kEditConstCmd)
		{
		(THXGetApplication())->ShowConstants();
		}
	else if (index == kNew2DPlotCmd)
		{
		(THXGetApplication())->New2DPlot(this);
		}
	else if (index == kNew3DPlotCmd)
		{
		(THXGetApplication())->New3DPlot();
		}
	else if (index == kConvBaseCmd)
		{
		(THXGetApplication())->ShowBaseConversion();
		}

	else if (index == kPSPageSetupCmd)
		{
		itsPlotWidget->HandlePSPageSetup();
		}
	else if (index == kPrintPSCmd)
		{
		itsPlotWidget->PrintPS();
		}

	else if (index == kPrintPlotEPSCmd)
		{
		itsPlotWidget->PrintPlotEPS();
		}
	else if (index == kPrintMarksEPSCmd)
		{
		itsPlotWidget->PrintMarksEPS();
		}

	else if (index == kCloseWindowCmd)
		{
		GetWindow()->Close();
		}
	else if (index == kQuitCmd)
		{
		(THXGetApplication())->Quit();
		}
}

/*******************************************************************************
 UpdateEditFnMenu

 ******************************************************************************/

void
THX2DPlotDirector::UpdateEditFnMenu()
{
	itsEditFnMenu->RemoveAllItems();

	const JSize count = itsFnList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsEditFnMenu->AppendItem(itsPlotWidget->GetCurveName(i));
		}
}

/*******************************************************************************
 HandleEditFnMenu

 ******************************************************************************/

void
THX2DPlotDirector::HandleEditFnMenu
	(
	const JIndex index
	)
{
	EditFunction(index);
}

/*******************************************************************************
 EditFunction (private)

 ******************************************************************************/

void
THX2DPlotDirector::EditFunction
	(
	const JIndex index
	)
{
	assert( itsEditFnDialog == NULL );

	const J2DPlotJFunction* curve = itsFnList->NthElement(index);

	JFloat min, max;
	curve->GetXRange(&min, &max);

	itsEditFnDialog =
		new THX2DPlotFunctionDialog(this, (THXGetApplication())->GetVariableList(),
									curve->GetFunction(),
									itsPlotWidget->GetCurveName(index), min, max);
	assert( itsEditFnDialog != NULL );
	itsEditFnDialog->BeginDialog();
	ListenTo(itsEditFnDialog);

	itsEditFnIndex = index;
}

/******************************************************************************
 UpdateFunction (private)

 ******************************************************************************/

void
THX2DPlotDirector::UpdateFunction()
{
	assert( itsEditFnDialog != NULL );

	JIndex plotIndex;
	const JFunction* f;
	JString curveName;
	JFloat xMin, xMax;
	itsEditFnDialog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

	J2DPlotJFunction* curve = itsFnList->NthElement(itsEditFnIndex);
	curve->SetFunction((THXGetApplication())->GetVariableList(), f->Copy(), kJTrue,
					   THXVarList::kXIndex, xMin, xMax);

	itsPlotWidget->SetCurveName(itsEditFnIndex, curveName);
}

/******************************************************************************
 UpdateCurveOptionsMenu (private)

 ******************************************************************************/

void
THX2DPlotDirector::UpdateCurveOptionsMenu()
{
	(itsPlotWidget->GetCurveOptionsMenu())->EnableItem(itsEditFunctionItemIndex);
}

/******************************************************************************
 HandleCurveOptionsMenu (private)

 ******************************************************************************/

void
THX2DPlotDirector::HandleCurveOptionsMenu
	(
	const JIndex index
	)
{
	if (index == itsEditFunctionItemIndex)
		{
		EditFunction(itsPlotWidget->GetCurveOptionsMenuCurveIndex());
		}
}
