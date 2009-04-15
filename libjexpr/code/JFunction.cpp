/******************************************************************************
 JFunction.cpp

	This abstract class defines the requirements for all Functions.

	BASE CLASS = none

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JFunction.h>
#include <jParseFunction.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JString.h>
#include <sstream>
#include <jAssert.h>

JFunction::JFunctionPrintDest JFunction::itsPrintDest = kStandardDest;

/******************************************************************************
 Constructor

 ******************************************************************************/

JFunction::JFunction
	(
	const JFunctionType type
	)
	:
	itsType( type )
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
	itsType( source.itsType )
{
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JFunction::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	return JConvertToBoolean( itsType == theFunction.itsType );
}

/******************************************************************************
 StreamIn (static)

	Parse the string to get the JFunction.

 ******************************************************************************/

JFunction*
JFunction::StreamIn
	(
	istream&				input,
	const JVariableList*	theVariableList,
	const JBoolean			allowUIF
	)
{
	JString expr;
	input >> expr;

	JFunction* theFunction;
	const JBoolean ok =
		JParseFunction(expr, theVariableList, &theFunction, allowUIF);
	assert( ok );
	return theFunction;
}

/******************************************************************************
 StreamOut

	Print the JFunction to the file as a JString.

 ******************************************************************************/

void
JFunction::StreamOut
	(
	ostream& output
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
	const JVariableList* newVariableList
	)
	const
{
	const JString expr = Print();
	JFunction* theFunction;
	const JBoolean ok = JParseFunction(expr, newVariableList, &theFunction);
	assert( ok );
	return theFunction;
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
 PrintForMathematica

 ******************************************************************************/

JString
JFunction::PrintForMathematica()
	const
{
	itsPrintDest = kMathematica;
	const JString result = Print();
	itsPrintDest = kStandardDest;
	return result;
}

void
JFunction::PrintForMathematica
	(
	ostream& output
	)
	const
{
	itsPrintDest = kMathematica;
	Print(output);
	itsPrintDest = kStandardDest;
}

/******************************************************************************
 PrepareToRender

	The default behavior is to display what Print displays.

 ******************************************************************************/

JIndex
JFunction::PrepareToRender
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

/******************************************************************************
 Cast to JFunctionWithArgs*

	Not inline because they are virtual

 ******************************************************************************/

JFunctionWithArgs*
JFunction::CastToJFunctionWithArgs()
{
	return NULL;
}

const JFunctionWithArgs*
JFunction::CastToJFunctionWithArgs()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JUnaryFunction*

	Not inline because they are virtual

 ******************************************************************************/

JUnaryFunction*
JFunction::CastToJUnaryFunction()
{
	return NULL;
}

const JUnaryFunction*
JFunction::CastToJUnaryFunction()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JBinaryFunction*

	Not inline because they are virtual

 ******************************************************************************/

JBinaryFunction*
JFunction::CastToJBinaryFunction()
{
	return NULL;
}

const JBinaryFunction*
JFunction::CastToJBinaryFunction()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JBinaryOperator*

	Not inline because they are virtual

 ******************************************************************************/

JBinaryOperator*
JFunction::CastToJBinaryOperator()
{
	return NULL;
}

const JBinaryOperator*
JFunction::CastToJBinaryOperator()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JNaryFunction*

	Not inline because they are virtual

 ******************************************************************************/

JNaryFunction*
JFunction::CastToJNaryFunction()
{
	return NULL;
}

const JNaryFunction*
JFunction::CastToJNaryFunction()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JNaryOperator*

	Not inline because they are virtual

 ******************************************************************************/

JNaryOperator*
JFunction::CastToJNaryOperator()
{
	return NULL;
}

const JNaryOperator*
JFunction::CastToJNaryOperator()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JFunctionWithVar*

	Not inline because they are virtual

 ******************************************************************************/

JFunctionWithVar*
JFunction::CastToJFunctionWithVar()
{
	return NULL;
}

const JFunctionWithVar*
JFunction::CastToJFunctionWithVar()
	const
{
	return NULL;
}

#define JTemplateType JFunction
#include <JPtrArray.tmpls>
#undef JTemplateType
