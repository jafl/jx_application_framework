/******************************************************************************
 THXExprDirector.cpp

	Window to display expression being edited by user.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THXExprDirector.h"
#include "THXExprEditor.h"
#include "THXTapeText.h"
#include "THXKeyPad.h"
#include "THXVarList.h"
#include "thxGlobals.h"
#include "thxHelpText.h"
#include "thxFileVersions.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXVertPartition.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXHelpManager.h>
#include <JXChooseSaveFile.h>
#include <JXMacWinPrefsDialog.h>
#include <JXImage.h>
#include <jXActionDefs.h>
#include <JFunction.h>
#include <fstream>
#include <sstream>
#include <jAssert.h>

JString	THXExprDirector::itsDefGeom;
const JFileVersion kCurrentDefGeomVersion = 0;

// Actions menu

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr =
	"    New expression      %k Meta-N"
	"  | Edit constants      %k Meta-E"
	"  | Plot 2D function... %k Meta-Shift-P"
	"  | Plot 3D function... %k Meta-Shift-O"
	"  | Base conversion...  %k Meta-B"
	"%l| Page setup..."
	"  | Print tape...       %k Meta-P"
	"%l| Save tape...        %k Meta-S"
	"  | Clear tape"
	"%l| Close window        %k Meta-W"
	"  | Quit                %k Meta-Q";

enum
{
	kNewExprCmd = 1,
	kEditConstCmd,
	kNew2DPlotCmd,
	kNew3DPlotCmd,
	kConvBaseCmd,
	kPageSetupCmd, kPrintTapeCmd,
	kSaveTapeCmd, kClearTapeCmd,
	kCloseWindowCmd, kQuitCmd
};

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Show keypad %b"
	"  | Mac/Win/X emulation..."
	"%l| Save window size as default";

enum
{
	kToggleKeyPadVisibleCmd = 1,
	kEditMacWinPrefsCmd,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

THXExprDirector::THXExprDirector
	(
	JXDirector*			supervisor,
	const THXVarList*	varList
	)
	:
	JXWindowDirector(supervisor),
	itsTapeName("Untitled")
{
	BuildWindow(varList);
	UseDefaultGeometry();
}

THXExprDirector::THXExprDirector
	(
	istream&			input,
	const JFileVersion	vers,
	JXDirector*			supervisor,
	const THXVarList*	varList
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow(varList);

	JFunction* f = JFunction::StreamIn(input, varList, kJTrue);
	itsExprWidget->SetFunction(varList, f);

	JString tapeText;
	input >> tapeText >> itsTapeName;
	itsTapeWidget->SetText(tapeText);

	if (vers >= 4)
		{
		JString printName;
		input >> printName;
		itsTapeWidget->SetPTPrintFileName(printName);
		}

	GetWindow()->ReadGeometry(input);
	itsPartition->ReadGeometry(input);
	itsExprWidget->ReadScrollSetup(input);
	itsTapeWidget->ReadScrollSetup(input);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXExprDirector::~THXExprDirector()
{
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
THXExprDirector::WriteState
	(
	ostream& output
	)
	const
{
	const JFunction* f = itsExprWidget->GetFunction();
	f->StreamOut(output);

	output << ' ' << itsTapeWidget->GetText();
	output << ' ' << itsTapeName;
	output << ' ' << itsTapeWidget->GetPTPrintFileName();

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsPartition->WriteGeometry(output);

	output << ' ';
	itsExprWidget->WriteScrollSetup(output);

	output << ' ';
	itsTapeWidget->WriteScrollSetup(output);
}

/******************************************************************************
 ReadPrefs (static)

 ******************************************************************************/

void
THXExprDirector::ReadPrefs
	(
	istream&			input,
	const JFileVersion	vers
	)
{
	if (vers >= 2)
		{
		input >> itsDefGeom;
		}
}

/******************************************************************************
 WritePrefs (static)

 ******************************************************************************/

void
THXExprDirector::WritePrefs
	(
	ostream& output
	)
{
	output << itsDefGeom;
}

/******************************************************************************
 UseDefaultGeometry (private)

 ******************************************************************************/

void
THXExprDirector::UseDefaultGeometry()
{
	if (!itsDefGeom.IsEmpty())
		{
		const std::string s(itsDefGeom.GetCString(), itsDefGeom.GetLength());
		std::istringstream input(s);

		JFileVersion vers;
		input >> vers;
		if (vers <= kCurrentDefGeomVersion)
			{
			JCoordinate w,h;
			input >> w >> h;
			GetWindow()->SetSize(w,h);

			itsPartition->ReadGeometry(input);
			}
		}
}

/******************************************************************************
 SetDefaultGeometry (private)

 ******************************************************************************/

void
THXExprDirector::SetDefaultGeometry()
{
	std::ostringstream data;
	data << kCurrentDefGeomVersion;
	data << ' ' << GetWindow()->GetFrameWidth();
	data << ' ' << GetWindow()->GetFrameHeight();
	data << ' ';
	itsPartition->WriteGeometry(data);

	itsDefGeom = data.str();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
THXExprDirector::Activate()
{
	JXWindowDirector::Activate();
	itsExprWidget->Focus();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_expr_window.xpm"

void
THXExprDirector::BuildWindow
	(
	const THXVarList* varList
	)
{
	JArray<JCoordinate> heights(2);
	heights.AppendElement(100);
	heights.AppendElement(0);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendElement(50);
	minHeights.AppendElement(50);

// begin JXLayout

	JXWindow* window = new JXWindow(this, 360,240, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 360,30);
	assert( menuBar != NULL );

	itsPartition =
		new JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 300,210);
	assert( itsPartition != NULL );

	itsKeyPad =
		new THXKeyPad(window,
					JXWidget::kFixedRight, JXWidget::kVElastic, 300,30, 60,210);
	assert( itsKeyPad != NULL );

// end JXLayout

	window->SetTitle("Leibnitz");
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(kJTrue);
	window->SetWMClass(THXGetWMClassInstance(), THXGetExprWindowClass());

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, thx_expr_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	// create tape

	JXScrollbarSet* scrollbarSet2 =
		new JXScrollbarSet(itsPartition->GetCompartment(2),
						   JXWidget::kHElastic, JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet2 != NULL );
	scrollbarSet2->FitToEnclosure();

	itsTapeWidget =
		new THXTapeText(scrollbarSet2, scrollbarSet2->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, 10,10);
	assert( itsTapeWidget != NULL );
	itsTapeWidget->FitToEnclosure();
	itsTapeWidget->SetPTPrinter(THXGetTapePrinter());

	// create expr editor -- requires tape

	JXScrollbarSet* scrollbarSet1 =
		new JXScrollbarSet(itsPartition->GetCompartment(1),
						   JXWidget::kHElastic, JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet1 != NULL );
	scrollbarSet1->FitToEnclosure();

	itsExprWidget =
		new THXExprEditor(varList, menuBar, itsTapeWidget, scrollbarSet1,
						  scrollbarSet1->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic,
						  0,0, 10,10);
	assert( itsExprWidget != NULL );
	itsExprWidget->FitToEnclosure();

	itsKeyPad->SetExprEditor(itsExprWidget);
	itsTapeWidget->ShareEditMenu(itsExprWidget->GetEditMenu());

	// create menus

	itsActionsMenu = menuBar->PrependTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = (THXGetApplication())->CreateHelpMenu(menuBar, "THXExprDirector");
	ListenTo(itsHelpMenu);

	// keypad visibility

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay

 ******************************************************************************/

void
THXExprDirector::UpdateDisplay()
{
	if (itsKeyPad->WouldBeVisible() && !(THXGetApplication())->KeyPadIsVisible())
		{
		itsKeyPad->Hide();
		itsPartition->AdjustSize(itsKeyPad->GetFrameWidth(), 0);
		}
	else if (!itsKeyPad->WouldBeVisible() && (THXGetApplication())->KeyPadIsVisible())
		{
		itsKeyPad->Show();
		itsPartition->AdjustSize(-itsKeyPad->GetFrameWidth(), 0);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
THXExprDirector::Receive
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

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
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
		(THXGetApplication())->HandleHelpMenu(itsHelpMenu, kTHXExprHelpName,
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
THXExprDirector::UpdateActionsMenu()
{
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
THXExprDirector::HandleActionsMenu
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
		(THXGetApplication())->New3DPlot();
		}
	else if (index == kConvBaseCmd)
		{
		(THXGetApplication())->ShowBaseConversion();
		}

	else if (index == kPageSetupCmd)
		{
		itsTapeWidget->HandlePTPageSetup();
		}
	else if (index == kPrintTapeCmd)
		{
		itsTapeWidget->PrintPT();
		}

	else if (index == kSaveTapeCmd)
		{
		SaveTape();
		}
	else if (index == kClearTapeCmd)
		{
		// this way, it is undoable

		itsTapeWidget->SelectAll();
		itsTapeWidget->DeleteSelection();
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
 SaveTape (private)

 ******************************************************************************/

void
THXExprDirector::SaveTape()
{
	JString newName;
	if ((JXGetChooseSaveFile())->SaveFile("Save tape as:", NULL,
										  itsTapeName, &newName))
		{
		itsTapeName = newName;

		ofstream output(itsTapeName);
		(itsTapeWidget->GetText()).Print(output);
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
THXExprDirector::UpdatePrefsMenu()
{
	if ((THXGetApplication())->KeyPadIsVisible())
		{
		itsPrefsMenu->CheckItem(kToggleKeyPadVisibleCmd);
		}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
THXExprDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kToggleKeyPadVisibleCmd)
		{
		(THXGetApplication())->ToggleKeyPadVisible();
		}
	else if (index == kEditMacWinPrefsCmd)
		{
		JXMacWinPrefsDialog::EditPrefs();
		}

	else if (index == kSaveWindSizeCmd)
		{
		SetDefaultGeometry();
		}
}
