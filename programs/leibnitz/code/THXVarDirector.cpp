/******************************************************************************
 THXVarDirector.cpp

	Window to display list of constants defined by user.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THXVarDirector.h"
#include "THXVarList.h"
#include "THXVarTable.h"
#include "thxGlobals.h"
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXExprInput.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXColHeaderWidget.h>
#include <jXActionDefs.h>
#include <JString.h>
#include <jAssert.h>

// Actions menu

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr =
	"    New constant        %k Meta-Shift-N"
	"  | Remove constant     %k Meta-R"
	"%l| New expression      %k Meta-N"
	"  | Plot 2D function... %k Meta-Shift-P"
	"  | Base conversion...  %k Meta-B"
	"%l| Close window        %k Meta-W"
	"  | Quit                %k Meta-Q";

enum
{
	kNewConstCmd = 1,
	kRemoveConstCmd,
	kNewExprCmd,
	kNew2DPlotCmd,
	kConvBaseCmd,
	kCloseWindowCmd,
	kQuitCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

THXVarDirector::THXVarDirector
	(
	JXDirector*	supervisor,
	THXVarList*	varList
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow(varList);
}

THXVarDirector::THXVarDirector
	(
	std::istream&			input,
	const JFileVersion	vers,
	JXDirector*			supervisor,
	THXVarList*			varList
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow(varList);

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsVarTable->ReadScrollSetup(input);

	JSize colWidth;
	input >> colWidth;
	itsVarTable->SetColWidth(THXVarTable::kNameColumn, colWidth);

	JBoolean active;
	input >> active;
	if (active)
		{
		Activate();
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXVarDirector::~THXVarDirector()
{
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
THXVarDirector::WriteState
	(
	std::ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsVarTable->WriteScrollSetup(output);

	output << ' ' << itsVarTable->GetColWidth(THXVarTable::kNameColumn);
	output << ' ' << JI2B(IsActive() && !GetWindow()->IsIconified());
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
THXVarDirector::BuildWindow
	(
	THXVarList* varList
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 260,250, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 260,30);
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,50, 260,200);
	assert( scrollbarSet != NULL );

	JXWidgetSet* colHdrContainer =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,30, 260,20);
	assert( colHdrContainer != NULL );

// end JXLayout

	window->SetTitle("Constants");
	window->SetWMClass(THXGetWMClassInstance(), THXGetVarWindowClass());
	window->SetMinSize(150,150);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(kJTrue);

	itsActionsMenu = menuBar->AppendTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	JXTextMenu* fontMenu = JXExprInput::CreateFontMenu(menuBar);
	menuBar->AppendMenu(fontMenu);

	itsHelpMenu = (THXGetApplication())->CreateHelpMenu(menuBar, "THXVarDirector");
	ListenTo(itsHelpMenu);

	itsVarTable =
		jnew THXVarTable(varList, fontMenu, scrollbarSet,
						scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, 10,10);
	assert( itsVarTable != NULL );

	JXColHeaderWidget* colHeader =
		jnew JXColHeaderWidget(itsVarTable, scrollbarSet, colHdrContainer,
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, "Name");
	colHeader->SetColTitle(2, "Value");
	colHeader->TurnOnColResizing(20);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
THXVarDirector::Receive
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

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		(THXGetApplication())->UpdateHelpMenu(itsHelpMenu);
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		(THXGetApplication())->HandleHelpMenu(itsHelpMenu, "THXConstantsHelp",
											  selection->GetIndex());
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
THXVarDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnable(kRemoveConstCmd,
								  itsVarTable->OKToRemoveSelectedConstant());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
THXVarDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kNewConstCmd)
		{
		itsVarTable->NewConstant();
		}
	else if (index == kRemoveConstCmd)
		{
		itsVarTable->RemoveSelectedConstant();
		}
	else if (index == kNewExprCmd)
		{
		(THXGetApplication())->NewExpression();
		}
	else if (index == kNew2DPlotCmd)
		{
		(THXGetApplication())->New2DPlot();
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

/******************************************************************************
 OKToDeactivate (virtual)

	Check that the table contents are valid.

	This is public so THXApp can call it.

 ******************************************************************************/

JBoolean
THXVarDirector::OKToDeactivate()
{
	return JXWindowDirector::OKToDeactivate();
}
