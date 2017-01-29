/******************************************************************************
 JFunctionWithVar.cpp

						The Function-with-a-Variable Class

	This abstract class provides routines for storing the reference to
	one variable.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JFunctionWithVar.h>
#include <JVariableList.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JExprNodeList.h>
#include <jParserData.h>

#include <JString.h>
#include <JListUtil.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be NULL

 ******************************************************************************/

JFunctionWithVar::JFunctionWithVar
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex,
	const JFunctionType		type
	)
	:
	JFunction(type)
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

	if (source.itsArrayIndex != NULL)
		{
		itsArrayIndex = (source.itsArrayIndex)->Copy();
		}
	else
		{
		itsArrayIndex = NULL;
		}

	itsVariableList->VariableUserCreated(this);
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.
	We don't compare the JVariableList pointers because this function can
	be called when comparing VariableLists.

 ******************************************************************************/

JBoolean
JFunctionWithVar::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunction::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JFunctionWithVar& theFunctionWithVar = (const JFunctionWithVar&) theFunction;

	if (itsVariableIndex != theFunctionWithVar.itsVariableIndex)
		{
		return kJFalse;
		}

	if (itsArrayIndex == NULL && theFunctionWithVar.itsArrayIndex == NULL)
		{
		return kJTrue;
		}
	else if (itsArrayIndex == NULL || theFunctionWithVar.itsArrayIndex == NULL)
		{
		return kJFalse;
		}
	else
		{
		return JConvertToBoolean( itsArrayIndex->SameAs(*(theFunctionWithVar.itsArrayIndex)) );
		}
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

	if (itsArrayIndex != NULL)
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
	if (itsArrayIndex != NULL && !itsArrayIndex->Evaluate(&x))
		{
		return kJFalse;
		}
	*index = JRound(x);
	if (!itsVariableList->ArrayIndexValid(itsVariableIndex, *index))
		{
		(JGetUserNotification())->ReportError("Array index out of bounds");
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JFunctionWithVar::PrepareToRender
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
	ourRect.right  = upperLeft.x + GetStringWidth(renderer, fontSize, baseName);

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
		ourRect.right += GetStringWidth(renderer, subFontSize, subscript);
		}

	// add in array index

	if (itsArrayIndex != NULL)
		{
		// get rectangle for array index

		JPoint argUpperLeft(ourRect.right, ourRect.top);
		const JIndex argIndex =
			itsArrayIndex->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);
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
	DrawString(renderer, ourRect.left, ourMidline, fontSize, baseName);

	// draw subscript

	if (!subscript.IsEmpty())
		{
		const JCoordinate subLeft =
			ourRect.left + GetStringWidth(renderer, fontSize, baseName);
		const JSize subFontSize = renderer.GetSuperSubFontSize(fontSize);
		const JSize subHeight   = renderer.GetLineHeight(subFontSize);
		DrawString(renderer, subLeft, ourMidline + subHeight/2, subFontSize, subscript);
		}

	// draw our array index

	if (itsArrayIndex != NULL)
		{
		itsArrayIndex->Render(renderer, rectList);

		JIndex argIndex;
		const JBoolean found = rectList.FindFunction(itsArrayIndex, &argIndex);
		assert( found );
		renderer.DrawSquareBrackets(rectList.GetRect(argIndex));
		}
}

/******************************************************************************
 GetStringWidth (private)

	Parse character following kGreekCharPrefix as a single greek character.

 ******************************************************************************/

JSize
JFunctionWithVar::GetStringWidth
	(
	const JExprRenderer&	renderer,
	const JSize				fontSize,
	const JString&			str
	)
	const
{
	JSize w = 0;

	const JCharacter* greekPrefix = JPGetGreekCharPrefixString();
	const JSize greekPrefixLength = JPGetGreekCharPrefixLength();

	JString s = str;
	JIndex greekIndex;
	while (s.LocateSubstring(greekPrefix, &greekIndex) &&
		   greekIndex < s.GetLength() - greekPrefixLength + 1)
		{
		if (greekIndex > 1)
			{
			const JString s1 = s.GetSubstring(1, greekIndex-1);
			w += renderer.GetStringWidth(fontSize, s1);
			}

		const JCharacter c = s.GetCharacter(greekIndex + greekPrefixLength);
		w += renderer.GetGreekCharWidth(fontSize, c);

		s.RemoveSubstring(1, greekIndex + greekPrefixLength);
		}

	if (!s.IsEmpty())
		{
		w += renderer.GetStringWidth(fontSize, s);
		}

	return w;
}

/******************************************************************************
 DrawString (private)

	Draw character following kGreekCharPrefix as a single greek character.

 ******************************************************************************/

void
JFunctionWithVar::DrawString
	(
	const JExprRenderer&	renderer,
	const JCoordinate		left,
	const JCoordinate		midline,
	const JSize				fontSize,
	const JString&			str
	)
	const
{
	JCoordinate x = left;

	const JCharacter* greekPrefix = JPGetGreekCharPrefixString();
	const JSize greekPrefixLength = JPGetGreekCharPrefixLength();

	JString s = str;
	JIndex greekIndex;
	while (s.LocateSubstring(greekPrefix, &greekIndex) &&
		   greekIndex < s.GetLength() - greekPrefixLength + 1)
		{
		if (greekIndex > 1)
			{
			const JString s1 = s.GetSubstring(1, greekIndex-1);
			renderer.DrawString(x, midline, fontSize, s1);
			x += renderer.GetStringWidth(fontSize, s1);
			}

		const JCharacter c = s.GetCharacter(greekIndex + greekPrefixLength);
		renderer.DrawGreekCharacter(x, midline, fontSize, c);
		x += renderer.GetGreekCharWidth(fontSize, c);

		s.RemoveSubstring(1, greekIndex + greekPrefixLength);
		}

	if (!s.IsEmpty())
		{
		renderer.DrawString(x, midline, fontSize, s);
		}
}

/******************************************************************************
 BuildNodeList

 ******************************************************************************/

void
JFunctionWithVar::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
	if (itsArrayIndex != NULL)
		{
		nodeList->RecurseNodesForFunction(myNode, itsArrayIndex);
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

/******************************************************************************
 Cast to JFunctionWithVar*

	Not inline because they are virtual

 ******************************************************************************/

JFunctionWithVar*
JFunctionWithVar::CastToJFunctionWithVar()
{
	return this;
}

const JFunctionWithVar*
JFunctionWithVar::CastToJFunctionWithVar()
	const
{
	return this;
}
