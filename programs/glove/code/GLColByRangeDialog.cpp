/******************************************************************************
 GLColByRangeDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLColByRangeDialog.h"
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

GLColByRangeDialog::GLColByRangeDialog
	(
	JXWindowDirector* supervisor,
	const JSize count
	)
	:
	JXDialogDirector(supervisor, true)
{
	itsDestCol = count;

	BuildWindow();	
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLColByRangeDialog::~GLColByRangeDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLColByRangeDialog::BuildWindow()
{
		
// begin JXLayout

	auto* window = jnew JXWindow(this, 210,190, JString::empty);
	assert( window != nullptr );

	itsBeginning =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,55, 100,20);
	assert( itsBeginning != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GLColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,160, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLColByRangeDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,160, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::GLColByRangeDialog::shortcuts::JXLayout"));

	itsEnd =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,90, 100,20);
	assert( itsEnd != nullptr );

	auto* maxLabel =
		jnew JXStaticText(JGetString("maxLabel::GLColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,90, 70,20);
	assert( maxLabel != nullptr );
	maxLabel->SetToLabel();

	auto* minLabel =
		jnew JXStaticText(JGetString("minLabel::GLColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,55, 70,20);
	assert( minLabel != nullptr );
	minLabel->SetToLabel();

	itsCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,125, 100,20);
	assert( itsCount != nullptr );

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::GLColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,125, 70,20);
	assert( countLabel != nullptr );
	countLabel->SetToLabel();

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::GLColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,15, 180,30);
	assert( itsDestMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLColByRangeDialog"));
	SetButtons(okButton, cancelButton);

	GLBuildColumnMenus("Column::GLGlobal", itsDestCol, itsDestMenu, nullptr);
	
	itsDestMenu->SetToPopupChoice(true, itsDestCol);
//	itsDestMenu->SetPopupChoice(itsDestCol);
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu);
	
	itsBeginning->SetIsRequired(true);
	itsEnd->SetIsRequired(true);
	itsCount->SetIsRequired(true);
	itsCount->SetLowerLimit(2);
	itsCount->SetValue(2);
}

/******************************************************************************
 GetDestination 

 ******************************************************************************/

void
GLColByRangeDialog::GetDestination
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
GLColByRangeDialog::Receive
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
GLColByRangeDialog::GetValues
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

bool 
GLColByRangeDialog::IsAscending()
{
	return itsIsAscending;
}

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

bool 
GLColByRangeDialog::OKToDeactivate()
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
		
	JFloat beg;
	JFloat end;
	JInteger count;

	itsBeginning->GetValue(&beg);
	itsEnd->GetValue(&end);

	itsIsAscending	= beg < end;
	itsCount->GetValue(&count);
	
	if (count <= 0)
		{
		JGetUserNotification()->ReportError(JGetString("NoData::GLColByIncDialog"));
		return false;
		}
		
	return true;
}
