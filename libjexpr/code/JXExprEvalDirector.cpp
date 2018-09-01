/******************************************************************************
 JXExprEvalDirector.cpp

	Class to display result of evaluating an expression in a JXExprEditor.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXExprEvalDirector.h"
#include "JVariableList.h"
#include "JFunction.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JString.h>
#include <JComplex.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

const JSize kSlopWidth = 20;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExprEvalDirector::JXExprEvalDirector
	(
	JXWindowDirector*		supervisor,
	const JVariableList*	varList,
	const JFunction&		f
	)
	:
	JXWindowDirector(supervisor)
{
	itsVarList = varList;
	ListenTo(itsVarList);

	itsFunction = f.Copy();

	BuildWindow(supervisor);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXExprEvalDirector::~JXExprEvalDirector()
{
	jdelete itsFunction;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXExprEvalDirector::BuildWindow
	(
	JXWindowDirector* supervisor
	)
{
	JXWindow* window = jnew JXWindow(this, 200,40, JString::empty);
	assert( window != nullptr );

	itsTextDisplay =
		jnew JXStaticText(JString::empty, kJFalse, kJTrue, nullptr, window,
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 10,10, 140,20);
	assert( itsTextDisplay != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("CloseButtonLabel::JXExprEvalDirector"), window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 150,10, 40,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("CloseButtonShortcuts::JXExprEvalDirector"));

	ListenTo(itsCloseButton);

	window->SetTitle(JGetString("WindowTitle::JXExprEvalDirector"));
	window->SetMinSize( 100, 40);
	window->SetMaxSize(5000, 40);
	UpdateDisplay();

	JXWindow* owner = supervisor->GetWindow();
	const JRect bG  = owner->GetBoundsGlobal();
	const JRect bR  = owner->GlobalToRoot(bG);
	window->Place(bR.left, bR.bottom - window->GetBoundsHeight());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXExprEvalDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == const_cast<JVariableList*>(itsVarList) &&
		(message.Is(JVariableList::kVarNameChanged) ||
		 message.Is(JVariableList::kVarValueChanged)))
		{
		UpdateDisplay();
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Close();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
JXExprEvalDirector::UpdateDisplay()
{
	JString str = itsFunction->Print();
	str += " = ";
	JComplex value;
	if (itsFunction->Evaluate(&value))
		{
		str += JPrintComplexNumber(value);
		}
	else
		{
		str += "error";
		}

	itsTextDisplay->GetText()->SetText(str);

	const JSize bdw = itsTextDisplay->GetBoundsWidth();
	const JSize apw = itsTextDisplay->GetApertureWidth();
	if (bdw > apw)
		{
		GetWindow()->AdjustSize(bdw - apw, 0);	// safe to calculate once bdw > apw
		}
	else if (apw > bdw + kSlopWidth)
		{
		GetWindow()->AdjustSize(apw - bdw, 0);	// safe to calculate once apw > bdw
		}
}
