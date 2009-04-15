/******************************************************************************
 GXColByRangeDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <glStdInc.h>
#include "GXColByRangeDialog.h"

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

GXColByRangeDialog::GXColByRangeDialog
	(
	JXWindowDirector* supervisor,
	const JSize count
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsDestCol = count;

	BuildWindow();	
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXColByRangeDialog::~GXColByRangeDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXColByRangeDialog::BuildWindow()
{
		
// begin JXLayout

    JXWindow* window = new JXWindow(this, 210,190, "");
    assert( window != NULL );
    SetWindow(window);

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
                    JXWidget::kHElastic, JXWidget::kVElastic, 15,165, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 125,165, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

    itsEnd =
        new JXFloatInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 95,90, 100,20);
    assert( itsEnd != NULL );

    JXStaticText* obj1 =
        new JXStaticText("End:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 50,95, 35,20);
    assert( obj1 != NULL );

    JXStaticText* obj2 =
        new JXStaticText("Start:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 47,60, 40,20);
    assert( obj2 != NULL );

    itsCount =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 95,125, 100,20);
    assert( itsCount != NULL );

    JXStaticText* obj3 =
        new JXStaticText("Count:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 39,130, 45,20);
    assert( obj3 != NULL );

// end JXLayout

	window->SetTitle("New Column by Range");
	SetButtons(okButton, cancelButton);

	for (JSize i = 1; i <= itsDestCol; i++)
		{
		JString str(i);
		str.Prepend("Column "); 
		itsDestMenu->AppendItem(str);
		}
	
	itsDestMenu->SetToPopupChoice(kJTrue, itsDestCol);
//	itsDestMenu->SetPopupChoice(itsDestCol);
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu);
	
	itsBeginning->SetIsRequired(kJTrue);
	itsEnd->SetIsRequired(kJTrue);
	itsCount->SetIsRequired(kJTrue);
	itsCount->SetLowerLimit(2);
	itsCount->SetValue(2);
}

/******************************************************************************
 GetDestination 

 ******************************************************************************/

void
GXColByRangeDialog::GetDestination
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
GXColByRangeDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsDestMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
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
GXColByRangeDialog::GetValues
	(
	JFloat* beg, 
	JFloat* end, 
	JInteger* count
	)
{
	itsBeginning->GetValue(beg);
	itsEnd->GetValue(end);
	itsCount->GetValue(count);
}

/******************************************************************************
 IsAscending 

 ******************************************************************************/

JBoolean 
GXColByRangeDialog::IsAscending()
{
	return itsIsAscending;
}

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

JBoolean 
GXColByRangeDialog::OKToDeactivate()
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
		
	JFloat beg;
	JFloat end;
	JInteger count;

	itsBeginning->GetValue(&beg);
	itsEnd->GetValue(&end);

	itsIsAscending	= JI2B(beg < end);
	itsCount->GetValue(&count);
	
	if (count <= 0)
		{
		JGetUserNotification()->ReportError("Count must be greater than zero.");
		return kJFalse;
		}
		
	return kJTrue;
}
