/******************************************************************************
 JNegation.cpp

								The Unary Minus Class

	This class returns the negative of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JNegation.h"
#include "jx-af/jexpr/JExprRenderer.h"
#include "jx-af/jexpr/JExprRectList.h"
#include "jx-af/jexpr/jFunctionUtil.h"
#include "jx-af/jexpr/JDivision.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JNegation::JNegation
	(
	JFunction* arg
	)
	:
	JUnaryFunction("-", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNegation::~JNegation()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JNegation::JNegation
	(
	const JNegation& source
	)
	:
	JUnaryFunction(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JNegation::Copy()
	const
{
	auto* newFunction = jnew JNegation(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JNegation::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat value;
	if (GetArg()->Evaluate(&value))
	{
		*result = -value;
		return true;
	}
	else
	{
		return false;
	}
}

bool
JNegation::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
	{
		*result = -value;
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JNegation::Print
	(
	std::ostream& output
	)
	const
{
	output << '-';

	const JFunction* arg = GetArg();
	if (JParenthesizeArgForPrint(*this, *arg))
	{
		output << '(';
		arg->Print(output);
		output << ')';
	}
	else
	{
		arg->Print(output);
	}
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JNegation::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle

	JPoint argUpperLeft = upperLeft;
	argUpperLeft.x += renderer.GetStringWidth(fontSize, JString("-", JString::kNoCopy));

	JFunction* arg = GetArg();
	if (dynamic_cast<JDivision*>(arg) != nullptr)
	{
		argUpperLeft.x += renderer.GetSpaceWidth(fontSize);
	}

	JRect ourRect(upperLeft, argUpperLeft);
	ourRect.bottom += renderer.GetLineHeight(fontSize);

	// get rectangle for our argument

	const JIndex argIndex =
		arg->Layout(renderer, argUpperLeft, fontSize, rectList);
	JRect argRect = rectList->GetRect(argIndex);

	if (JParenthesizeArgForRender(*this, *arg))
	{
		const JSize parenWidth = renderer.GetParenthesisWidth(argRect.height());
		rectList->ShiftRect(argIndex, parenWidth, 0);
		argRect = rectList->GetRect(argIndex);
		ourRect.right = argRect.right + parenWidth;
	}

	ourRect = JCovering(ourRect, argRect);
	const JCoordinate ourMidline = rectList->GetMidline(argIndex);

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JNegation::Render
	(
	const JExprRenderer& renderer,
	const JExprRectList& rectList
	)
	const
{
	// find ourselves in the list

	JIndex ourIndex;
	const bool found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);
	const JCoordinate ourMidline = rectList.GetMidline(ourIndex);
	const JSize fontSize = rectList.GetFontSize(ourIndex);

	// draw ourselves

	renderer.DrawString(ourRect.left, ourMidline, fontSize, JString("-", JString::kNoCopy));

	const JFunction* arg = GetArg();
	arg->Render(renderer, rectList);

	if (JParenthesizeArgForRender(*this, *arg))
	{
		JIndex argIndex;
		const bool found = rectList.FindFunction(arg, &argIndex);
		assert( found );
		renderer.DrawParentheses(rectList.GetRect(argIndex));
	}
}
