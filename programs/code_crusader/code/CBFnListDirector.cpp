/******************************************************************************
 CBFnListDirector.cpp

	Class to display the functions defined by a CBClass.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "CBFnListDirector.h"
#include "CBFnListWidget.h"
#include "CBTreeDirector.h"
#include "CBClass.h"
#include "CBProjectDocument.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXScrollbarSet.h>
#include <JXPSPrinter.h>
#include <JTableSelection.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

// Actions menu

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr =
	"    Show inherited functions      %b"
	"  | Select pure virtual functions"
	"  | Copy selected function names     %k Meta-C"
	"%l| Update                           %k Meta-U"
	"  | Show class tree                  %k Meta-Shift-C"
	"%l| Page setup..."
	"  | Print...                         %k Meta-P"
	"%l| Close window                     %k Meta-W"
	"  | Close all                        %k Meta-Shift-W";

enum
{
	kShowInheritedCmd = 1, kSelectPureVirtualCmd,
	kCopySelFnNamesCmd,
	kUpdateCmd, kShowTreeCmd,
	kPageSetupCmd, kPrintCmd,
	kCloseWindowCmd, kCloseAllCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFnListDirector::CBFnListDirector
	(
	CBTreeDirector*		supervisor,
	JXPSPrinter*		printer,
	const CBClass*		theClass,
	const CBTreeWidget*	treeWidget,
	const JBoolean		showInheritedFns,
	const JBoolean		forPopupMenu
	)
	:
	JXWindowDirector(supervisor)
{
	JXScrollbarSet* scrollbarSet = BuildWindow();

	itsFnListWidget =
		jnew CBFnListWidget(treeWidget, theClass, showInheritedFns, scrollbarSet,
							scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 10,10);

	CBFnListDirectorX(supervisor, printer, forPopupMenu, kJTrue);
}

CBFnListDirector::CBFnListDirector
	(
	std::istream&			input,
	const JFileVersion	vers,
	JBoolean*			keep,
	CBTreeDirector*		supervisor,
	JXPSPrinter*		printer,
	const CBTreeWidget*	treeWidget
	)
	:
	JXWindowDirector(supervisor)
{
	JXScrollbarSet* scrollbarSet = BuildWindow();
	GetWindow()->ReadGeometry(input);

	itsFnListWidget =
		jnew CBFnListWidget(input, vers, keep, treeWidget, scrollbarSet,
							scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 10,10);

	CBFnListDirectorX(supervisor, printer, kJFalse, *keep);

	if (vers >= 24)
		{
		// must read even if !keep so next guy won't fail

		itsFnListWidget->ReadScrollSetup(input);
		}
}

// private

void
CBFnListDirector::CBFnListDirectorX
	(
	CBTreeDirector*		director,
	JXPSPrinter*		printer,
	const JBoolean		forPopupMenu,
	const JBoolean		willBeKept
	)
{
	assert( itsFnListWidget != NULL );
	itsFnListWidget->FitToEnclosure();
	if (willBeKept)
		{
		GetWindow()->SetTitle((itsFnListWidget->GetClass())->GetFullName());
		}

	itsDirector = director;
	itsPrinter  = printer;

	if (!forPopupMenu)
		{
		itsDirector->FnBrowserCreated(this);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFnListDirector::~CBFnListDirector()
{
}

/******************************************************************************
 IsShowingClass

 ******************************************************************************/

JBoolean
CBFnListDirector::IsShowingClass
	(
	const CBClass* theClass
	)
	const
{
	return JConvertToBoolean( itsFnListWidget->GetClass() == theClass );
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
CBFnListDirector::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << ' ';
	GetWindow()->WriteGeometry(output);
	output << ' ';
	itsFnListWidget->StreamOut(output);
	output << ' ';
	itsFnListWidget->WriteScrollSetup(output);	// after FitToEnclosure()
	output << ' ';
}

/******************************************************************************
 SkipSetup (static)

 ******************************************************************************/

void
CBFnListDirector::SkipSetup
	(
	std::istream& input
	)
{
	JXWindow::SkipGeometry(input);
	CBFnListWidget::SkipSetup(input);
	CBFnListWidget::SkipScrollSetup(input);
}

/******************************************************************************
 Reconnect

 ******************************************************************************/

void
CBFnListDirector::Reconnect()
{
	if (!itsFnListWidget->Reconnect())
		{
		Close();
		}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

JXScrollbarSet*
CBFnListDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 170,250, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 170,220);
	assert( scrollbarSet != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 170,30);
	assert( menuBar != NULL );

// end JXLayout

	window->SetMinSize(150,150);

	itsActionsMenu = menuBar->AppendTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr, "CBFnListDirector");
	ListenTo(itsActionsMenu);

	return scrollbarSet;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBFnListDirector::Receive
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

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsFnListWidget->Print(*itsPrinter);
			}
		StopListening(itsPrinter);
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
CBFnListDirector::UpdateActionsMenu()
{
	itsActionsMenu->EnableItem(kShowInheritedCmd);
	if (itsFnListWidget->IsShowingInheritedFns())
		{
		itsActionsMenu->CheckItem(kShowInheritedCmd);
		}

	if (itsFnListWidget->HasUnimplementedFns())
		{
		itsActionsMenu->EnableItem(kSelectPureVirtualCmd);
		}

	if ((itsFnListWidget->GetTableSelection()).HasSelection())
		{
		itsActionsMenu->EnableItem(kCopySelFnNamesCmd);
		}

	itsActionsMenu->EnableItem(kUpdateCmd);
	itsActionsMenu->EnableItem(kShowTreeCmd);

	if (itsPrinter != NULL)
		{
		itsActionsMenu->EnableItem(kPageSetupCmd);
		itsActionsMenu->EnableItem(kPrintCmd);
		}

	itsActionsMenu->EnableItem(kCloseWindowCmd);
	itsActionsMenu->EnableItem(kCloseAllCmd);
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
CBFnListDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kShowInheritedCmd)
		{
		itsFnListWidget->ToggleShowInheritedFns();
		}
	else if (index == kSelectPureVirtualCmd)
		{
		itsFnListWidget->SelectUnimplementedFns();
		}
	else if (index == kCopySelFnNamesCmd)
		{
		itsFnListWidget->CopySelectedFunctionNames();
		}

	else if (index == kUpdateCmd)
		{
		itsDirector->GetProjectDoc()->UpdateSymbolDatabase();
		}
	else if (index == kShowTreeCmd)
		{
		itsDirector->Activate();
		}

	else if (index == kPageSetupCmd && itsPrinter != NULL)
		{
		itsPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintCmd && itsPrinter != NULL)
		{
		itsPrinter->BeginUserPrintSetup();
		ListenTo(itsPrinter);
		}

	else if (index == kCloseWindowCmd)
		{
		Close();
		}
	else if (index == kCloseAllCmd)
		{
		itsDirector->CloseFunctionBrowsers();
		}
}
