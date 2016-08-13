/******************************************************************************
 THX3DPlotDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THX3DPlotDirector.h"
#include "THX3DPlotWidget.h"
#include "THX3DPlotFunctionDialog.h"
#include "THXVarList.h"
#include "thxGlobals.h"
#include "thxHelpText.h"
#include <J3DUniverse.h>
#include <J3DSurface.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <JXDeleteObjectTask.h>
#include <jXActionDefs.h>
#include <JMatrix.h>
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
	"%l| Close window        %k Meta-W"
	"  | Quit                %k Meta-Q";

enum
{
	kNewExprCmd = 1,
	kEditConstCmd,
	kNew2DPlotCmd,
	kNew3DPlotCmd,
	kConvBaseCmd,
	kCloseWindowCmd, kQuitCmd
};

// Options menu

static const JCharacter* kOptionsMenuTitleStr = "Options";
static const JCharacter* kOptionsMenuStr =
	"    Edit function...";
/*
	"%l| Change scale..."
	"  | Reset Z scale"
	"%l| Show frame  %b"
	"  | Show grid   %b"
	"  | Show legend %b"
	"%l| Change labels...";
*/
enum
{
	kEditFunctionCmd = 1,
	kScaleCmd,
	kResetZScaleCmd,
	kShowFrameCmd,
	kShowGridCmd,
	kShowLegendCmd,
	kChangeLabelsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

THX3DPlotDirector::THX3DPlotDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	THX3DPlotDirectorX(NULL);
	BuildWindow();
}

THX3DPlotDirector::THX3DPlotDirector
	(
	istream&			input,
	const JFileVersion	vers,
	JXDirector*			supervisor,
	THXVarList*			varList
	)
	:
	JXWindowDirector(supervisor)
{
	assert( vers >= 10 );

	THX3DPlotDirectorX(varList);

	BuildWindow();
	GetWindow()->ReadGeometry(input);

	JString surfaceName;
	input >> surfaceName;
	GetWindow()->SetTitle(surfaceName);

	input >> itsXMin >> itsXMax >> itsYMin >> itsYMax;
	input >> itsXCount >> itsYCount;

	itsFunction = JFunction::StreamIn(input, varList);

	ComputeFunction();
}

// private

void
THX3DPlotDirector::THX3DPlotDirectorX
	(
	THXVarList* varList
	)
{
	itsVarList      = varList;
	itsFunction     = NULL;
	itsEditFnDialog = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THX3DPlotDirector::~THX3DPlotDirector()
{
	JXDeleteObjectTask<J3DObject>::Delete(itsUniverse);
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
THX3DPlotDirector::WriteState
	(
	ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	output << ' ';
	output << GetWindow()->GetTitle();

	output << ' ' << itsXMin << ' ' << itsXMax;
	output << ' ' << itsYMin << ' ' << itsYMax;

	output << ' ' << itsXCount << ' ' << itsYCount;

	output << ' ';
	itsFunction->StreamOut(output);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_3D_plot_window.xpm"

void
THX3DPlotDirector::BuildWindow()
{
	// create a universe

	JXDisplay* d = (JXGetApplication())->GetCurrentDisplay();

	itsUniverse = new J3DUniverse(*(d->GetColormap()));
	assert( itsUniverse != NULL );

// begin JXLayout

	JXWindow* window = new JXWindow(this, 450,480, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != NULL );

	its3DPlot =
		new THX3DPlotWidget(itsUniverse, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,450);
	assert( its3DPlot != NULL );

// end JXLayout

	window->SetMinSize(300, 200);
	window->SetWMClass(THXGetWMClassInstance(), THXGetPlotWindowClass());

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, thx_3D_plot_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	its3DPlot->ShouldShowFocus(kJFalse);

	itsActionsMenu = menuBar->PrependTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsOptionsMenu = menuBar->AppendTextMenu(kOptionsMenuTitleStr);
	assert( itsOptionsMenu != NULL );
	itsOptionsMenu->SetMenuItems(kOptionsMenuStr);
	itsOptionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsOptionsMenu);

	itsHelpMenu = (THXGetApplication())->CreateHelpMenu(menuBar, "THX3DPlotDirector");
	ListenTo(itsHelpMenu);
}

/******************************************************************************
 SetFunction

 ******************************************************************************/

void
THX3DPlotDirector::SetFunction
	(
	THXVarList*			varList,
	const JFunction&	f,
	const JCharacter*	name,
	const JFloat		xMin,
	const JFloat		xMax,
	const JSize			xCount,
	const JFloat		yMin,
	const JFloat		yMax,
	const JSize			yCount
	)
{
	itsVarList = varList;

	delete itsFunction;
	itsFunction = f.Copy();

	itsXMin = xMin;
	itsXMax = xMax;
	itsYMin = yMin;
	itsYMax = yMax;

	itsXCount = xCount;
	itsYCount = yCount;

	GetWindow()->SetTitle(name);

	ComputeFunction();
}

/******************************************************************************
 ComputeFunction (private)

	The perturbation is a hack to try to avoid the infinities and
	divide-by-zeros that inevitably happen in interesting functions.

 ******************************************************************************/

const JFloat kShiftPerturbation = 1e-6;
const JFloat kScalePerturbation = 1.0 + kShiftPerturbation;

void
THX3DPlotDirector::ComputeFunction()
{
	const JSize ptCount = itsXCount*itsYCount + (itsXCount-1)*(itsYCount-1);

	JMatrix* data =
		J3DSurface::ComputeGrid(itsXMin, itsXMax, itsXCount,
								itsYMin, itsYMax, itsYCount);

	const JSize rowCount = data->GetRowCount();
	for (JIndex i=1; i<=rowCount; i++)
		{
		const JFloat x = data->GetElement(i, 1);
		const JFloat y = data->GetElement(i, 2);

		itsVarList->SetNumericValue(THXVarList::kXIndex, 1, x*kScalePerturbation + kShiftPerturbation);
		itsVarList->SetNumericValue(THXVarList::kYIndex, 1, y*kScalePerturbation + kShiftPerturbation);

		JFloat z;
		if (!itsFunction->Evaluate(&z))
			{
			z = NAN;
			}

		data->SetElement(i, 3, z);
		}

	its3DPlot->PlotData(itsXCount, itsYCount, data);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
THX3DPlotDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsOptionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateOptionsMenu();
		}
	else if (sender == itsOptionsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleOptionsMenu(selection->GetIndex());
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
		(THXGetApplication())->HandleHelpMenu(itsHelpMenu, kTHX3DPlotHelpName,
											  selection->GetIndex());
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
THX3DPlotDirector::UpdateActionsMenu()
{
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
THX3DPlotDirector::HandleActionsMenu
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
		(THXGetApplication())->New2DPlot();
		}
	else if (index == kNew3DPlotCmd)
		{
		(THXGetApplication())->New3DPlot(this);
		}
	else if (index == kConvBaseCmd)
		{
		(THXGetApplication())->ShowBaseConversion();
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
 UpdateOptionsMenu

 ******************************************************************************/

void
THX3DPlotDirector::UpdateOptionsMenu()
{
//	if (FrameIsVisible())
		{
//		itsOptionsMenu->CheckItem(kShowFrameCmd);
		}

//	if (GridIsVisible())
		{
//		itsOptionsMenu->CheckItem(kShowGridCmd);
		}

//	if (LegendIsVisible())
		{
//		itsOptionsMenu->CheckItem(kShowLegendCmd);
		}
}

/*******************************************************************************
 HandleOptionsMenu

 ******************************************************************************/

void
THX3DPlotDirector::HandleOptionsMenu
	(
	const JIndex index
	)
{
	if (index == kEditFunctionCmd)
		{
		EditFunction();
		}

	else if (index == kScaleCmd)
		{
//		ChangeScale();
		}
	else if (index == kResetZScaleCmd)
		{
//		ResetZScale();
		}

	else if (index == kShowFrameCmd)
		{
		}
	else if (index == kShowGridCmd)
		{
		}
	else if (index == kShowLegendCmd)
		{
		}

	else if (index == kChangeLabelsCmd)
		{
//		ChangeLabels(kNoLabel);
		}
}

/*******************************************************************************
 EditFunction (private)

 ******************************************************************************/

void
THX3DPlotDirector::EditFunction()
{
	assert( itsEditFnDialog == NULL );

	itsEditFnDialog =
		new THX3DPlotFunctionDialog(this, itsVarList, *itsFunction,
									GetWindow()->GetTitle(),
									itsXMin, itsXMax, itsXCount,
									itsYMin, itsYMax, itsYCount);
	assert( itsEditFnDialog != NULL );
	itsEditFnDialog->BeginDialog();
	ListenTo(itsEditFnDialog);
}

/******************************************************************************
 UpdateFunction (private)

 ******************************************************************************/

void
THX3DPlotDirector::UpdateFunction()
{
	assert( itsEditFnDialog != NULL );

	const JFunction* f;
	JString surfaceName;
	itsEditFnDialog->GetSettings(&f, &surfaceName,
								 &itsXMin, &itsXMax, &itsXCount,
								 &itsYMin, &itsYMax, &itsYCount);

	delete itsFunction;
	itsFunction = f->Copy();

	GetWindow()->SetTitle(surfaceName);

	ComputeFunction();
}
