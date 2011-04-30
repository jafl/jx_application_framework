/******************************************************************************
 GXColByIncDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <glStdInc.h>
#include "GXColByIncDialog.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXFloatInput.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>

#include <JUserNotification.h>
#include <JString.h>

#include <jGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GXColByIncDialog::GXColByIncDialog
	(
	JXWindowDirector* supervisor,
	const JSize count
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
	
	for (JSize i = 1; i <= count; i++)
		{
		JString str(i);
		str.Prepend("Column "); 
		itsDestMenu->AppendItem(str);
		}
	
	itsDestMenu->SetToPopupChoice(kJTrue, count);
	itsDestCol = count;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXColByIncDialog::~GXColByIncDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXColByIncDialog::BuildWindow()
{
		
// begin JXLayout

    JXWindow* window = new JXWindow(this, 220,190, "");
    assert( window != NULL );

    itsDestMenu =
        new JXTextMenu("Destination:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 15,15, 160,30);
    assert( itsDestMenu != NULL );

    itsBeginning =
        new JXFloatInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 95,55, 100,20);
    assert( itsBeginning != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 15,160, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 125,160, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

    itsInc =
        new JXFloatInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 95,90, 100,20);
    assert( itsInc != NULL );

    JXStaticText* obj1 =
        new JXStaticText("Increment:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 19,95, 70,20);
    assert( obj1 != NULL );

    JXStaticText* obj2 =
        new JXStaticText("Start:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 48,60, 40,20);
    assert( obj2 != NULL );

    itsCount =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 95,125, 100,20);
    assert( itsCount != NULL );

    JXStaticText* obj3 =
        new JXStaticText("Count:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 40,130, 45,20);
    assert( obj3 != NULL );

// end JXLayout

	window->SetTitle("New Column by Increment");
	SetButtons(okButton, cancelButton);
	
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu);
	
	itsBeginning->SetIsRequired(kJTrue);
	itsInc->SetIsRequired(kJTrue);
	itsCount->SetIsRequired(kJTrue);
	itsCount->SetLowerLimit(2);
	itsCount->SetValue(2);
}

/******************************************************************************
 GetDestination 

 ******************************************************************************/

void
GXColByIncDialog::GetDestination
	(
	JIndex* dest
	)
{
	*dest = itsDestCol;
}

/******************************************************************************
 Receive 

 ******************************************************************************/

void
GXColByIncDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsDestMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsDestCol = selection->GetIndex();
		}
		
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetValues 

 ******************************************************************************/

void 
GXColByIncDialog::GetValues
	(
	JFloat* beg, 
	JFloat* inc, 
	JInteger* count
	)
{
	itsBeginning->GetValue(beg);
	itsInc->GetValue(inc);
	itsCount->GetValue(count);
}

/******************************************************************************
 IsAscending 

 ******************************************************************************/

JBoolean 
GXColByIncDialog::IsAscending()
{
	JFloat inc;
	itsInc->GetValue(&inc);
	if (inc > 0)
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

JBoolean 
GXColByIncDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}
		
	if (itsDestCol == 0)
		{
		JGetUserNotification()->ReportError("You must specify the destination column.");
		return kJFalse;
		}
		
	JInteger count;
	
	itsCount->GetValue(&count);
	
	if (count <= 0)
		{
		JGetUserNotification()->ReportError("Count must be greater than zero.");
		return kJFalse;
		}
		
	return kJTrue;
}
