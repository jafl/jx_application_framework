/******************************************************************************
 THX2DPlotDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THX2DPlotDirector.h"
#include "THX2DPlotFunctionDialog.h"
#include "THXVarList.h"
#include "thxGlobals.h"
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
#include <JExprParser.h>
#include <JFunction.h>
#include <jAssert.h>

// Actions menu

static const JUtf8Byte* kActionsMenuStr =
	"    New expression      %k Meta-N"
	"  | Edit constants      %k Meta-E"
	"  | Plot 2D function... %k Meta-Shift-P"
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
	kConvBaseCmd,
	kPSPageSetupCmd, kPrintPSCmd,
	kPrintPlotEPSCmd, kPrintMarksEPSCmd,
	kCloseWindowCmd, kQuitCmd
};

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
	std::istream&		input,
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

	JExprParser p(varList);
	JString expr;

	for (JIndex i=1; i<=fnCount; i++)
		{
		JFloat xMin, xMax;
		input >> xMin >> xMax;

		input >> expr;

		JFunction* f;
		const bool ok = p.Parse(expr, &f);
		assert( ok );

		AddFunction(varList, f, JString::empty, xMin, xMax);
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
	itsFnList = jnew JPtrArray<J2DPlotJFunction>(JPtrArrayT::kForgetAll);
	assert( itsFnList != nullptr );

	itsEditFnDialog = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THX2DPlotDirector::~THX2DPlotDirector()
{
	jdelete itsFnList;	// contents not owned
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
THX2DPlotDirector::WriteState
	(
	std::ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	const JSize fnCount = itsPlotWidget->GetCurveCount();
	output << ' ' << fnCount;

	for (JIndex i=1; i<=fnCount; i++)
		{
		const JPlotDataBase& data      = itsPlotWidget->GetCurve(i);
		const auto* fnData = dynamic_cast<const J2DPlotJFunction*>(&data);
		assert( fnData != nullptr );

		JFloat xMin, xMax;
		fnData->GetXRange(&xMin, &xMax);
		output << ' ' << xMin << ' ' << xMax;

		output << ' ' << fnData->GetFunction().Print();
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

	auto* window = jnew JXWindow(this, 600,400, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( menuBar != nullptr );

	itsPlotWidget =
		jnew JX2DPlotWidget(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,370);
	assert( itsPlotWidget != nullptr );

// end JXLayout

	window->SetMinSize(300, 200);
	window->SetWMClass(THXGetWMClassInstance(), THXGetPlotWindowClass());

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, thx_2D_plot_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	itsPlotWidget->SetPSPrinter(THXGetPSGraphPrinter());
	itsPlotWidget->SetEPSPrinter(THXGetEPSGraphPrinter());

	ListenTo(itsPlotWidget);

	itsActionsMenu = menuBar->PrependTextMenu(JGetString("ActionsMenuTitle::thxGlobals"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	JXTextMenu* optionsMenu               = itsPlotWidget->GetOptionsMenu();
	const JIndex editFunctionSubmenuIndex = optionsMenu->GetItemCount()+1;
	optionsMenu->AppendItem(JGetString("EditFunctionItem::THX2DPlotDirector"));

	itsEditFnMenu = jnew JXTextMenu(optionsMenu, editFunctionSubmenuIndex, menuBar);
	assert( itsEditFnMenu != nullptr );
	itsEditFnMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsEditFnMenu);

	itsHelpMenu = (THXGetApplication())->CreateHelpMenu(menuBar, "THX2DPlotDirector");
	ListenTo(itsHelpMenu);

	JXTextMenu* curveOptionsMenu = itsPlotWidget->GetCurveOptionsMenu();
	itsEditFunctionItemIndex     = curveOptionsMenu->GetItemCount()+1;
	curveOptionsMenu->ShowSeparatorAfter(itsEditFunctionItemIndex-1);
	curveOptionsMenu->AppendItem(JGetString("EditFunctionItem::THX2DPlotDirector"));
	ListenTo(curveOptionsMenu);

	// do this after everything is constructed so Receive() doesn't crash

	itsPlotWidget->SetTitle(JGetString("PlotTitle::THX2DPlotDirector"));
}

/******************************************************************************
 AddFunction

 ******************************************************************************/

void
THX2DPlotDirector::AddFunction
	(
	THXVarList*			varList,
	const JFunction&	f,
	const JString&		name,
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
	THXVarList*		varList,
	JFunction*		f,
	const JString&	name,
	const JFloat	xMin,
	const JFloat	xMax
	)
{
	auto* data =
		jnew J2DPlotJFunction(itsPlotWidget, varList, f, true,
							 THXVarList::kXIndex, xMin, xMax);
	assert( data != nullptr );

	itsPlotWidget->AddCurve(data, true, name);

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
		if (sender == itsFnList->GetElement(i))
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsEditFnMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEditFnMenu();
		}
	else if (sender == itsEditFnMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleEditFnMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		(THXGetApplication())->UpdateHelpMenu(itsHelpMenu);
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		(THXGetApplication())->HandleHelpMenu(itsHelpMenu, "THX2DPlotHelp",
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleCurveOptionsMenu(selection->GetIndex());
		}

	else if (sender == itsEditFnDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateFunction();
			}
		itsEditFnDialog = nullptr;
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
	assert( itsEditFnDialog == nullptr );

	const J2DPlotJFunction* curve = itsFnList->GetElement(index);

	JFloat min, max;
	curve->GetXRange(&min, &max);

	itsEditFnDialog =
		jnew THX2DPlotFunctionDialog(this, (THXGetApplication())->GetVariableList(),
									curve->GetFunction(),
									itsPlotWidget->GetCurveName(index), min, max);
	assert( itsEditFnDialog != nullptr );
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
	assert( itsEditFnDialog != nullptr );

	JIndex plotIndex;
	const JFunction* f;
	JString curveName;
	JFloat xMin, xMax;
	itsEditFnDialog->GetSettings(&plotIndex, &f, &curveName, &xMin, &xMax);

	J2DPlotJFunction* curve = itsFnList->GetElement(itsEditFnIndex);
	curve->SetFunction((THXGetApplication())->GetVariableList(), f->Copy(), true,
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
