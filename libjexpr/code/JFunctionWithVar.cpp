/******************************************************************************
 JFunctionWithVar.cpp

						The Function-with-a-Variable Class

	This abstract class provides routines for storing the reference to
	one variable.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JFunctionWithVar.h>
#include <JVariableList.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>

#include <JString.h>
#include <JListUtil.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be nullptr

 ******************************************************************************/

JFunctionWithVar::JFunctionWithVar
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex
	)
{
	itsVariableList  = theVariableList;
	itsVariableIndex = variableIndex;
	itsArrayIndex    = arrayIndex;

	itsVariableList->VariableUserCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFunctionWithVar::~JFunctionWithVar()
{
	itsVariableList->VariableUserDeleted(this);

	jdelete itsArrayIndex;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JFunctionWithVar::JFunctionWithVar
	(
	const JFunctionWithVar& source
	)
	:
	JFunction(source)
{
	itsVariableList  = source.itsVariableList;
	itsVariableIndex = source.itsVariableIndex;

	if (source.itsArrayIndex != nullptr)
		{
		itsArrayIndex = (source.itsArrayIndex)->Copy();
		}
	else
		{
		itsArrayIndex = nullptr;
		}

	itsVariableList->VariableUserCreated(this);
}

/******************************************************************************
 PrintVariable

 ******************************************************************************/

void
JFunctionWithVar::PrintVariable
	(
	std::ostream& output
	)
	const
{
	const JString& name = itsVariableList->GetVariableName(itsVariableIndex);
	name.Print(output);

	if (itsArrayIndex != nullptr)
		{
		output << '[';
		itsArrayIndex->Print(output);
		output << ']';
		}
}

/******************************************************************************
 SetVariableIndex

	Derived classes can override this if they only allow certain types.
	Not inlined because it is virtual.

 ******************************************************************************/

void
JFunctionWithVar::SetVariableIndex
	(
	const JIndex variableIndex
	)
{
	itsVariableIndex = variableIndex;
}

/******************************************************************************
 EvaluateArrayIndex

 ******************************************************************************/

JBoolean
JFunctionWithVar::EvaluateArrayIndex
	(
	JIndex* index
	)
	const
{
	JFloat x = 1.0;
	if (itsArrayIndex != nullptr && !itsArrayIndex->Evaluate(&x))
		{
		return kJFalse;
		}
	*index = JRound(x);
	if (!itsVariableList->ArrayIndexValid(itsVariableIndex, *index))
		{
		(JGetUserNotification())->ReportError(JGetString("ArrayIndexOutOfBounds::JFunctionWithVar"));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JFunctionWithVar::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle and midline

	JString baseName, subscript;
	itsVariableList->GetVariableName(itsVariableIndex, &baseName, &subscript);

	JRect ourRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.bottom = upperLeft.y + renderer.GetLineHeight(fontSize);
	ourRect.right  = upperLeft.x + renderer.GetStringWidth(fontSize, baseName);

	JCoordinate ourMidline = ourRect.ycenter();

	// add in subscript

	if (!subscript.IsEmpty())
		{
		const JSize subFontSize     = renderer.GetSuperSubFontSize(fontSize);
		const JCoordinate subHeight = renderer.GetLineHeight(subFontSize);
		if (ourMidline + subHeight > ourRect.bottom)
			{
			ourRect.bottom = ourMidline + subHeight;
			}
		ourRect.right += renderer.GetStringWidth(subFontSize, subscript);
		}

	// add in array index

	if (itsArrayIndex != nullptr)
		{
		// get rectangle for array index

		JPoint argUpperLeft(ourRect.right, ourRect.top);
		const JIndex argIndex =
			itsArrayIndex->Layout(renderer, argUpperLeft, fontSize, rectList);
		ourRect    = JCovering(ourRect, rectList->GetRect(argIndex));
		ourMidline = rectList->GetMidline(argIndex);

		// shift array index to make space for left bracket

		const JSize bracketWidth = renderer.GetSquareBracketWidth(ourRect.height());
		rectList->ShiftRect(argIndex, bracketWidth, 0);

		// we need space for two brackets

		ourRect.right += 2*bracketWidth;
		}

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JFunctionWithVar::Render
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

	// draw base name

	JString baseName, subscript;
	itsVariableList->GetVariableName(itsVariableIndex, &baseName, &subscript);
	renderer.DrawString(ourRect.left, ourMidline, fontSize, baseName);

	// draw subscript

	if (!subscript.IsEmpty())
		{
		const JCoordinate subLeft =
			ourRect.left + renderer.GetStringWidth(fontSize, baseName);
		const JSize subFontSize = renderer.GetSuperSubFontSize(fontSize);
		const JSize subHeight   = renderer.GetLineHeight(subFontSize);
		renderer.DrawString(subLeft, ourMidline + subHeight/2, subFontSize, subscript);
		}

	// draw our array index

	if (itsArrayIndex != nullptr)
		{
		itsArrayIndex->Render(renderer, rectList);

		JIndex argIndex;
		const JBoolean found = rectList.FindFunction(itsArrayIndex, &argIndex);
		assert( found );
		renderer.DrawSquareBrackets(rectList.GetRect(argIndex));
		}
}

/******************************************************************************
 JVariableList messages (virtual)

 ******************************************************************************/

JBoolean
JFunctionWithVar::UsesVariable
	(
	const JIndex variableIndex
	)
	const
{
	return JI2B( itsVariableIndex == variableIndex ||
				 JFunction::UsesVariable(variableIndex) );
}

void
JFunctionWithVar::VariablesInserted
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	JFunction::VariablesRemoved(firstIndex, count);
	JAdjustIndexAfterInsert(firstIndex, count, &itsVariableIndex);
}

void
JFunctionWithVar::VariablesRemoved
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	JFunction::VariablesRemoved(firstIndex, count);
	const JBoolean ok = JAdjustIndexAfterRemove(firstIndex, count, &itsVariableIndex);
	assert( ok );	// JVariableList must ensure this
}

void
JFunctionWithVar::VariableMoved
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	JFunction::VariableMoved(origIndex, newIndex);
	JAdjustIndexAfterMove(origIndex, newIndex, &itsVariableIndex);
}

void
JFunctionWithVar::VariablesSwapped
	(
	const JIndex index1,
	const JIndex index2
	)
{
	JFunction::VariablesSwapped(index1, index2);
	JAdjustIndexAfterSwap(index1, index2, &itsVariableIndex);
}
