/******************************************************************************
 JFunction.cpp

	This abstract class defines the requirements for all Functions.

	BASE CLASS = none

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JFunction.h"
#include "JExprRenderer.h"
#include "JExprRectList.h"
#include "JExprParser.h"
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFunction::JFunction()
	:
	itsParent(nullptr)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFunction::~JFunction()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JFunction::JFunction
	(
	const JFunction& source
	)
	:
	itsParent(nullptr)
{
}

/******************************************************************************
 StreamIn (static)

	Parse the string to get the JFunction.

 ******************************************************************************/

JFunction*
JFunction::StreamIn
	(
	std::istream&			input,
	const JVariableList*	varList,
	JFontManager*			fontManager,
	const JBoolean			allowUIF
	)
{
	JString expr;
	input >> expr;

	JExprParser p(expr, varList, fontManager, allowUIF);

	const int result = p.yyparse();
	assert( result == 0 );

	return p.GetFunction();
}

/******************************************************************************
 StreamOut

	Print the JFunction to the file as a JString.

 ******************************************************************************/

void
JFunction::StreamOut
	(
	std::ostream& output
	)
	const
{
	const JString str = Print();
	output << str;
}

/******************************************************************************
 Copy

	Create a copy of ourselves that uses a different JVariableList.

	The easiest way to do this is to print and then reparse ourselves
	using the new JVariableList.

 ******************************************************************************/

JFunction*
JFunction::Copy
	(
	const JVariableList*	newVariableList,
	JFontManager*			fontManager
	)
	const
{
	const JString expr = Print();

	JExprParser p(expr, newVariableList, fontManager);

	const int result = p.yyparse();
	assert( result == 0 );

	return p.GetFunction();
}

/******************************************************************************
 Print

 ******************************************************************************/

JString
JFunction::Print()
	const
{
	std::ostringstream expr;
	Print(expr);
	return JString(expr.str());
}

/******************************************************************************
 Layout

	The default behavior is to display what Print displays.

 ******************************************************************************/

JIndex
JFunction::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	const JString text = Print();

	JRect ourRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.bottom = upperLeft.y + renderer.GetLineHeight(fontSize);
	ourRect.right  = upperLeft.x + renderer.GetStringWidth(fontSize, text);

	const JCoordinate ourMidline = ourRect.ycenter();
	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

	The default behavior is to display what Print displays.

 ******************************************************************************/

void
JFunction::Render
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

	// display the output of Print

	const JString text = Print();
	renderer.DrawString(ourRect.left, ourMidline, fontSize, text);
}

/******************************************************************************
 JVariableList messages (virtual)

	The default is to do nothing.  Derived classes that store variable
	indicies must override all the functions.

 ******************************************************************************/

JBoolean
JFunction::UsesVariable
	(
	const JIndex variableIndex
	)
	const
{
	return kJFalse;
}

void
JFunction::VariablesInserted
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
}

void
JFunction::VariablesRemoved
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
}

void
JFunction::VariableMoved
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
}

void
JFunction::VariablesSwapped
	(
	const JIndex index1,
	const JIndex index2
	)
{
}
