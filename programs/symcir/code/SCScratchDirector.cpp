/******************************************************************************
 SCScratchDirector.cpp

	BASE CLASS = SCDirectorBase

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCScratchDirector.h"
#include "SCCircuitDocument.h"
#include "SCExprEditor.h"
#include "SCExprEditorSet.h"
#include "SCEditScratchTitleDialog.h"

#include <JXWindow.h>
#include <JXTextMenu.h>

#include <JString.h>
#include <jAssert.h>

static const JCharacter* kChangeTitleItemStr = "Change window title";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCScratchDirector::SCScratchDirector
	(
	SCCircuitDocument* supervisor
	)
	:
	SCDirectorBase(supervisor)
{
	BuildWindow(supervisor);
}

SCScratchDirector::SCScratchDirector
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuitDocument*	supervisor
	)
	:
	SCDirectorBase(input, vers, supervisor)
{
	BuildWindow(supervisor);

	JXWindow* w = GetWindow();
	w->ReadGeometry(input);

	JString title;
	input >> title;
	w->SetTitle(title);

	itsFunction->ReadState(input, vers);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCScratchDirector::~SCScratchDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SCScratchDirector::BuildWindow
	(
	SCCircuitDocument* doc
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 350,130, "");
    assert( window != NULL );

    SCExprEditorSet* obj1_JXLayout =
        new SCExprEditorSet(doc, &itsFunction, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 350,130);
    assert( obj1_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Scratch window");
	window->LockCurrentMinSize();

	// add our menu items

	JXTextMenu* menu = itsFunction->GetExtraMenu();
	menu->AppendItem(kChangeTitleItemStr);
	itsChangeTitleIndex = menu->GetItemCount();
	menu->ShowSeparatorAfter(itsChangeTitleIndex-1);
	ListenTo(menu);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCScratchDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* menu = itsFunction->GetExtraMenu();

	if (sender == menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateExtraMenu();
		}
	else if (sender == menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleExtraMenu(selection->GetIndex());
		}

	else
		{
		SCDirectorBase::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateExtraMenu (private)

 ******************************************************************************/

void
SCScratchDirector::UpdateExtraMenu()
{
	JXTextMenu* menu = itsFunction->GetExtraMenu();
	menu->EnableItem(itsChangeTitleIndex);
}

/******************************************************************************
 HandleExtraMenu (private)

 ******************************************************************************/

void
SCScratchDirector::HandleExtraMenu
	(
	const JIndex item
	)
{
	if (item == itsChangeTitleIndex)
		{
		JXDialogDirector* dlog = new SCEditScratchTitleDialog(this);
		assert( dlog != NULL );
		dlog->BeginDialog();
		}
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
SCScratchDirector::StreamOut
	(
	ostream& output
	)
	const
{
	output << ' ' << kScratchType;

	JXWindow* w = GetWindow();

	output << ' ';
	w->WriteGeometry(output);

	output << ' ' << w->GetTitle();
	output << ' ';
	itsFunction->WriteState(output);

	output << ' ';
}
