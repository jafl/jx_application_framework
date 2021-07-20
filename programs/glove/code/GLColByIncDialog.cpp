/******************************************************************************
 GLColByIncDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLColByIncDialog.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXFloatInput.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>

#include <JUserNotification.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLColByIncDialog::GLColByIncDialog
	(
	JXWindowDirector*	supervisor,
	const JSize			count
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow();
	
	GLBuildColumnMenus("Column::GLGlobal", count, itsDestMenu, nullptr);
	
	itsDestMenu->SetToPopupChoice(true, count);
	itsDestCol = count;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLColByIncDialog::~GLColByIncDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLColByIncDialog::BuildWindow()
{
		
// begin JXLayout

	auto* window = jnew JXWindow(this, 220,190, JString::empty);
	assert( window != nullptr );

	itsBeginning =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,55, 100,20);
	assert( itsBeginning != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GLColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,160, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLColByIncDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,160, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::GLColByIncDialog::shortcuts::JXLayout"));

	itsInc =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,90, 100,20);
	assert( itsInc != nullptr );

	auto* incrLabel =
		jnew JXStaticText(JGetString("incrLabel::GLColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,90, 70,20);
	assert( incrLabel != nullptr );
	incrLabel->SetToLabel();

	auto* minLabel =
		jnew JXStaticText(JGetString("minLabel::GLColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,55, 70,20);
	assert( minLabel != nullptr );
	minLabel->SetToLabel();

	itsCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,125, 100,20);
	assert( itsCount != nullptr );

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::GLColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,125, 70,20);
	assert( countLabel != nullptr );
	countLabel->SetToLabel();

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::GLColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,15, 180,30);
	assert( itsDestMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLColByIncDialog"));
	SetButtons(okButton, cancelButton);
	
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu);
	
	itsBeginning->SetIsRequired(true);
	itsInc->SetIsRequired(true);
	itsCount->SetIsRequired(true);
	itsCount->SetLowerLimit(2);
	itsCount->SetValue(2);
}

/******************************************************************************
 GetDestination 

 ******************************************************************************/

void
GLColByIncDialog::GetDestination
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
GLColByIncDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsDestMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
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
GLColByIncDialog::GetValues
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

bool 
GLColByIncDialog::IsAscending()
{
	JFloat inc;
	itsInc->GetValue(&inc);
	if (inc > 0)
		{
		return true;
		}
	return false;
}

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

bool 
GLColByIncDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return true;
		}
		
	if (itsDestCol == 0)
		{
		JGetUserNotification()->ReportError(JGetString("MissingDestCol::GLColByIncDialog"));
		return false;
		}
		
	JInteger count;
	
	itsCount->GetValue(&count);
	
	if (count <= 0)
		{
		JGetUserNotification()->ReportError(JGetString("NoData::GLColByIncDialog"));
		return false;
		}
		
	return true;
}
