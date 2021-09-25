/******************************************************************************
 JFunctionWithArgs.cpp

						The JFunction with Arguments Class

	This abstract class defines the requirements for all functions that
	take arguments.  We also provide a default implementation of Print.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JFunctionWithArgs.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFunctionWithArgs::JFunctionWithArgs
	(
	const JUtf8Byte* name
	)
	:
	itsName(name)
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JFunctionWithArgs::JFunctionWithArgs
	(
	const JFunctionWithArgs& source
	)
	:
	JFunction(source),
	itsName(source.itsName)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFunctionWithArgs::~JFunctionWithArgs()
{
}

/******************************************************************************
 ReplaceArg

	Returns true if origArg was found in the list of arguments.

 ******************************************************************************/

bool
JFunctionWithArgs::ReplaceArg
	(
	const JFunction*	origArg,
	JFunction*			newArg
	)
{
	bool argReplaced = false;
	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
	{
		if (GetArg(i) == origArg)
		{
			SetArg(i, newArg);
			argReplaced = true;
			break;
		}
	}
	return argReplaced;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JFunctionWithArgs::Print
	(
	std::ostream& output
	)
	const
{
	GetName().Print(output);
	output << '(';
	PrintArgs(output);
	output << ')';
}

// private

void
JFunctionWithArgs::PrintArgs
	(
	std::ostream& output
	)
	const
{
	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
	{
		(GetArg(i))->Print(output);
		if (i < argCount)
		{
			output << ",";
		}
	}
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JFunctionWithArgs::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle and midline

	JRect ourRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.bottom = upperLeft.y + renderer.GetLineHeight(fontSize);
	ourRect.right  = upperLeft.x + renderer.GetStringWidth(fontSize, itsName);

	JCoordinate ourMidline = ourRect.ycenter();
	const JCoordinate origMidline = ourMidline;

	// get rectangle for each argument

	JPoint argUpperLeft(ourRect.right, ourRect.top);
	const JSize sepWidth = renderer.GetStringWidth(fontSize, JString(", ", JString::kNoCopy));

	const JSize argCount = GetArgCount();
{
	for (JIndex i=1; i<=argCount; i++)
	{
		JFunction* arg = GetArg(i);
		const JIndex argIndex =
			arg->Layout(renderer, argUpperLeft, fontSize, rectList);
		const JRect argRect = rectList->GetRect(argIndex);
		argUpperLeft.x = argRect.right + sepWidth;
		ourRect        = JCovering(ourRect, argRect);
		const JCoordinate argMidline = rectList->GetMidline(argIndex);
		if (argMidline > ourMidline)
		{
			ourMidline = argMidline;
		}
	}
}

	// adjust the argument rectangles so all the midlines are the same
	// (ourMidline is guaranteed to stay constant)

	if (argCount > 1 && ourMidline > origMidline)
	{
		for (JIndex i=1; i<=argCount; i++)
		{
			const JFunction* arg = GetArg(i);
			JIndex argIndex;
			const bool found = rectList->FindFunction(arg, &argIndex);
			assert( found );
			rectList->SetMidline(argIndex, ourMidline);
			ourRect = JCovering(ourRect, rectList->GetRect(argIndex));
		}
	}

	// Now that the midlines are the same, the height of our rectangle is
	// the height of the parentheses.  We have to shift all the arguments
	// to the right to make space for the left parenthesis.  By shifting
	// the rightmost one first, we avoid overlapping anything.

	const JSize parenWidth = renderer.GetParenthesisWidth(ourRect.height());
{
	for (JIndex i=argCount; i>=1; i--)
	{
		const JFunction* arg = GetArg(i);
		JIndex argIndex;
		const bool found = rectList->FindFunction(arg, &argIndex);
		assert( found );
		rectList->ShiftRect(argIndex, parenWidth, 0);
	}
}

	// we need space for two parentheses

	ourRect.right += 2*parenWidth;

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JFunctionWithArgs::Render
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

	renderer.DrawString(ourRect.left, ourMidline, fontSize, itsName);

	JRect parenRect;
	parenRect.top    = ourRect.top;
	parenRect.bottom = ourRect.bottom;

	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
	{
		const JFunction* arg = GetArg(i);
		arg->Render(renderer, rectList);

		JIndex argIndex;
		const bool found = rectList.FindFunction(arg, &argIndex);
		assert( found );
		const JRect argRect = rectList.GetRect(argIndex);
		if (i == 1)
		{
			parenRect.left = argRect.left;
		}
		if (i < argCount)
		{
			renderer.DrawString(argRect.right, ourMidline, fontSize, JString(",", JString::kNoCopy));
		}
		else
		{
			parenRect.right = argRect.right;
			renderer.DrawParentheses(parenRect);
		}
	}
}
