/******************************************************************************
 GXColByIncDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

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

	JXWindow* window = jnew JXWindow(this, 220,190, "");
	assert( window != NULL );

	itsBeginning =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,55, 100,20);
	assert( itsBeginning != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::GXColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,160, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GXColByIncDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GXColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,160, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GXColByIncDialog::shortcuts::JXLayout"));

	itsInc =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,90, 100,20);
	assert( itsInc != NULL );

	JXStaticText* obj1_JXLayout =
		jnew JXStaticText(JGetString("obj1_JXLayout::GXColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,90, 70,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		jnew JXStaticText(JGetString("obj2_JXLayout::GXColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,55, 70,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,125, 100,20);
	assert( itsCount != NULL );

	JXStaticText* obj3_JXLayout =
		jnew JXStaticText(JGetString("obj3_JXLayout::GXColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,125, 70,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	JXTextMenu* obj4_JXLayout =
		jnew JXTextMenu(JGetString("obj4_JXLayout::GXColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,15, 180,30);
	assert( obj4_JXLayout != NULL );

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
