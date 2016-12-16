/******************************************************************************
 JFunctionWithArgs.cpp

						The JFunction with Arguments Class

	This abstract class defines the requirements for all functions that
	take arguments.  We also provide a default implementation of Print.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JFunctionWithArgs.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JExprNodeList.h>
#include <jParserData.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFunctionWithArgs::JFunctionWithArgs
	(
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JFunction(type),
	itsNameIndex(nameIndex)
{
	assert( 1 <= nameIndex && nameIndex <= kJFnNameCount );
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
	itsNameIndex(source.itsNameIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFunctionWithArgs::~JFunctionWithArgs()
{
}

/******************************************************************************
 GetName

 ******************************************************************************/

const JCharacter*
JFunctionWithArgs::GetName()
	const
{
	return JPGetStdFnName(itsNameIndex);
}

/******************************************************************************
 GetMathematicaName

 ******************************************************************************/

const JCharacter*
JFunctionWithArgs::GetMathematicaName()
	const
{
	return JPGetMathematicaFnName(itsNameIndex);
}

/******************************************************************************
 ReplaceArg

	Returns kJTrue if origArg was found in the list of arguments.

 ******************************************************************************/

JBoolean
JFunctionWithArgs::ReplaceArg
	(
	const JFunction*	origArg,
	JFunction*			newArg
	)
{
	JBoolean argReplaced = kJFalse;
	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		if (GetArg(i) == origArg)
			{
			SetArg(i, newArg);
			argReplaced = kJTrue;
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
	const JFunctionPrintDest dest = GetPrintDestination();
	if (dest == kMathematica)
		{
		output << GetMathematicaName();
		}
	else
		{
		output << GetName();
		}
	PrintArgs(output);
	if (dest == kMathematica)
		{
		output << ']';
		}
	else
		{
		output << ')';
		}
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
			output << JPGetArgSeparatorString() << ' ';
			}
		}
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JFunctionWithArgs::PrepareToRender
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle and midline

	const JCharacter* name = GetName();
	const JSize nameLength = strlen(name);
	assert( nameLength > 1 );
	assert( name[ nameLength-1 ] == '(' );
	const JString fnName = JString(name, nameLength-1);

	JRect ourRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.bottom = upperLeft.y + renderer.GetLineHeight(fontSize);
	ourRect.right  = upperLeft.x + renderer.GetStringWidth(fontSize, fnName);

	JCoordinate ourMidline = ourRect.ycenter();
	const JCoordinate origMidline = ourMidline;

	// get rectangle for each argument

	JPoint argUpperLeft(ourRect.right, ourRect.top);
	const JSize sepWidth = renderer.GetStringWidth(fontSize, JPGetArgSeparatorString()) +
						   renderer.GetStringWidth(fontSize, " ");

	const JSize argCount = GetArgCount();
	{
	for (JIndex i=1; i<=argCount; i++)
		{
		JFunction* arg = GetArg(i);
		const JIndex argIndex =
			arg->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);
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
			const JBoolean found = rectList->FindFunction(arg, &argIndex);
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
		const JBoolean found = rectList->FindFunction(arg, &argIndex);
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
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);
	const JCoordinate ourMidline = rectList.GetMidline(ourIndex);
	const JSize fontSize = rectList.GetFontSize(ourIndex);

	// draw ourselves

	const JCharacter* name = GetName();
	const JSize nameLength = strlen(name);
	assert( nameLength > 1 );
	assert( name[ nameLength-1 ] == '(' );
	const JString fnName = JString(name, nameLength-1);
	renderer.DrawString(ourRect.left, ourMidline, fontSize, fnName);

	JRect parenRect;
	parenRect.top    = ourRect.top;
	parenRect.bottom = ourRect.bottom;

	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		const JFunction* arg = GetArg(i);
		arg->Render(renderer, rectList);

		JIndex argIndex;
		const JBoolean found = rectList.FindFunction(arg, &argIndex);
		assert( found );
		const JRect argRect = rectList.GetRect(argIndex);
		if (i == 1)
			{
			parenRect.left = argRect.left;
			}
		if (i < argCount)
			{
			renderer.DrawString(argRect.right, ourMidline, fontSize, JPGetArgSeparatorString());
			}
		else
			{
			parenRect.right = argRect.right;
			renderer.DrawParentheses(parenRect);
			}
		}
}

/******************************************************************************
 BuildNodeList

 ******************************************************************************/

void
JFunctionWithArgs::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		nodeList->RecurseNodesForFunction(myNode, GetArg(i));
		}
}

/******************************************************************************
 Cast to JFunctionWithArgs*

	Not inline because they are virtual

 ******************************************************************************/

JFunctionWithArgs*
JFunctionWithArgs::CastToJFunctionWithArgs()
{
	return this;
}

const JFunctionWithArgs*
JFunctionWithArgs::CastToJFunctionWithArgs()
	const
{
	return this;
}
