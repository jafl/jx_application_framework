/******************************************************************************
 THXKeyPad.cpp

	BASE CLASS = JXWidgetSet

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <thxStdInc.h>
#include "THXKeyPad.h"
#include "THXExprEditor.h"
#include <JXTextButton.h>
#include <jGlobals.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

// remember to keep the enum up to date

static const JCharacter kOpKey[] =
{
	'.', 'e', '+', '-', '*', '/', '=', kJEscapeKey
};

/******************************************************************************
 Constructor

 ******************************************************************************/

THXKeyPad::THXKeyPad
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsExpr = NULL;
	CreateButtons();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXKeyPad::~THXKeyPad()
{
}

/******************************************************************************
 CreateButtons (private)

 ******************************************************************************/

void
THXKeyPad::CreateButtons()
{
// begin kpLayout

	const JRect kpLayout_Frame    = this->GetFrame();
	const JRect kpLayout_Aperture = this->GetAperture();
	this->AdjustSize(60 - kpLayout_Aperture.width(), 160 - kpLayout_Aperture.height());

	itsDigitButton[7] =
		new JXTextButton(JGetString("itsDigitButton[7]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 20,20);
	assert( itsDigitButton[7] != NULL );

	itsDigitButton[8] =
		new JXTextButton(JGetString("itsDigitButton[8]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,0, 20,20);
	assert( itsDigitButton[8] != NULL );

	itsDigitButton[9] =
		new JXTextButton(JGetString("itsDigitButton[9]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,0, 20,20);
	assert( itsDigitButton[9] != NULL );

	itsDigitButton[4] =
		new JXTextButton(JGetString("itsDigitButton[4]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,20, 20,20);
	assert( itsDigitButton[4] != NULL );

	itsDigitButton[5] =
		new JXTextButton(JGetString("itsDigitButton[5]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 20,20);
	assert( itsDigitButton[5] != NULL );

	itsDigitButton[6] =
		new JXTextButton(JGetString("itsDigitButton[6]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,20, 20,20);
	assert( itsDigitButton[6] != NULL );

	itsDigitButton[1] =
		new JXTextButton(JGetString("itsDigitButton[1]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,40, 20,20);
	assert( itsDigitButton[1] != NULL );

	itsDigitButton[2] =
		new JXTextButton(JGetString("itsDigitButton[2]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 20,20);
	assert( itsDigitButton[2] != NULL );

	itsDigitButton[3] =
		new JXTextButton(JGetString("itsDigitButton[3]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,40, 20,20);
	assert( itsDigitButton[3] != NULL );

	itsDigitButton[0] =
		new JXTextButton(JGetString("itsDigitButton[0]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,60, 20,20);
	assert( itsDigitButton[0] != NULL );

	itsOpButton[kPtOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kPtOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 20,20);
	assert( itsOpButton[kPtOpIndex] != NULL );

	itsOpButton[kExpOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kExpOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,60, 20,20);
	assert( itsOpButton[kExpOpIndex] != NULL );

	itsClearAllButton =
		new JXTextButton(JGetString("itsClearAllButton::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,140, 60,20);
	assert( itsClearAllButton != NULL );

	itsOpButton[kClearOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kClearOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,120, 60,20);
	assert( itsOpButton[kClearOpIndex] != NULL );

	itsOpButton[kEvalOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kEvalOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,80, 20,40);
	assert( itsOpButton[kEvalOpIndex] != NULL );

	itsOpButton[kMultiplyOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kMultiplyOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,100, 20,20);
	assert( itsOpButton[kMultiplyOpIndex] != NULL );

	itsOpButton[kDivideOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kDivideOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 20,20);
	assert( itsOpButton[kDivideOpIndex] != NULL );

	itsOpButton[kAddOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kAddOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,80, 20,20);
	assert( itsOpButton[kAddOpIndex] != NULL );

	itsOpButton[kSubtractOpIndex] =
		new JXTextButton(JGetString("itsOpButton[kSubtractOpIndex]::THXKeyPad::kpLayout"), this,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 20,20);
	assert( itsOpButton[kSubtractOpIndex] != NULL );

	this->SetSize(kpLayout_Frame.width(), kpLayout_Frame.height());

// end kpLayout

	itsOpButton[kMultiplyOpIndex]->SetLabel("\xD7");
	itsOpButton[kDivideOpIndex]->SetLabel("\xF7");

JIndex i;

	for (i=0; i<10; i++)
		{
		ListenTo(itsDigitButton[i]);
		}

	for (i=0; i<kOpCount; i++)
		{
		ListenTo(itsOpButton[i]);
		}

	ListenTo(itsClearAllButton);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
THXKeyPad::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JXButton::kPushed) && HandleButton(sender))
		{
		// work is done by function
		}
	else
		{
		JXWidgetSet::Receive(sender, message);
		}
}

/******************************************************************************
 HandleButton (private)

 ******************************************************************************/

JBoolean
THXKeyPad::HandleButton
	(
	JBroadcaster* sender
	)
	const
{
JIndex i;
JXKeyModifiers modifiers(GetDisplay());

	if (itsExpr == NULL || !itsExpr->Focus())
		{
		return kJFalse;
		}

	for (i=0; i<10; i++)
		{
		if (sender == itsDigitButton[i])
			{
			if (!itsExpr->HasSelection() && !itsExpr->UIFIsActive())
				{
				if (itsExpr->ContainsUIF())
					{
					itsExpr->ActivateNextUIF();
					}
				else
					{
					itsExpr->SelectAll();
					}
				}

			itsExpr->HandleKeyPress('0' + i, modifiers);
			return kJTrue;
			}
		}

	for (i=0; i<kOpCount; i++)
		{
		if (sender == itsOpButton[i])
			{
			if (!itsExpr->HasSelection() && !itsExpr->UIFIsActive())
				{
				itsExpr->SelectAll();
				}

			itsExpr->HandleKeyPress(kOpKey[i], modifiers);
			return kJTrue;
			}
		}

	if (sender == itsClearAllButton)
		{
		itsExpr->CancelEditing();
		itsExpr->SelectAll();
		itsExpr->DeleteSelection();
		return kJTrue;
		}

	return kJFalse;
}
