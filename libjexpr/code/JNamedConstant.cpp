/******************************************************************************
 JNamedConstant.cpp

							The Named Constant Class

	This class represents a named constant numeric value.

	Since we only store the index of the constant, this can't be used
	for anything other than constants defined at compile time.
	User-defined values must be represented by something else.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JNamedConstant.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <jParserData.h>
#include <jMath.h>
#include <string.h>
#include <jAssert.h>

const JCharacter kGreekPiChar = 'p';

static const JFloat kNamedConstValues[] =
{
	kJPi, kJE, 0.0
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JNamedConstant::JNamedConstant
	(
	const JNamedConstIndex nameIndex
	)
	:
	JFunction(kJNamedConstantType)
{
	assert( 1 <= nameIndex && nameIndex <= kJNamedConstCount );
	itsNameIndex = nameIndex;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNamedConstant::~JNamedConstant()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JNamedConstant::JNamedConstant
	(
	const JNamedConstant& source
	)
	:
	JFunction(source)
{
	itsNameIndex = source.itsNameIndex;
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JNamedConstant::Copy()
	const
{
	JNamedConstant* newFunction = jnew JNamedConstant(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JNamedConstant::Evaluate
	(
	JFloat* result
	)
	const
{
	if (itsNameIndex == kIJNamedConstIndex)		// i is not a real number
		{
		*result = 0.0;
		return kJFalse;
		}
	else
		{
		*result = kNamedConstValues[ itsNameIndex-1 ];
		return kJTrue;
		}
}

JBoolean
JNamedConstant::Evaluate
	(
	JComplex* result
	)
	const
{
	if (itsNameIndex == kIJNamedConstIndex)		// i is not a real number
		{
		*result = JComplex(0.0, 1.0);
		}
	else
		{
		*result = kNamedConstValues[ itsNameIndex-1 ];
		}

	return kJTrue;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JNamedConstant::Print
	(
	ostream& output
	)
	const
{
	if (GetPrintDestination() == kMathematica)
		{
		output << JPGetMathematicaNamedConstName(itsNameIndex);
		}
	else if (itsNameIndex == kIJNamedConstIndex)
		{
		output << JGetCurrentImagString();
		}
	else
		{
		output << JPGetStdNamedConstName(itsNameIndex);
		}
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JNamedConstant::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunction::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JNamedConstant& theNamedConstant = (const JNamedConstant&) theFunction;
	return JConvertToBoolean( itsNameIndex == theNamedConstant.itsNameIndex );
}

/******************************************************************************
 BuildNodeList

	We are a terminal node.

 ******************************************************************************/

void
JNamedConstant::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
}

/******************************************************************************
 PrepareToRender

	Draw pi using the Greek symbol.

 ******************************************************************************/

JIndex
JNamedConstant::PrepareToRender
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	if (strcmp(JPGetStdNamedConstName(itsNameIndex), JPGetPiString()) == 0)
		{
		JRect ourRect;
		ourRect.top    = upperLeft.y;
		ourRect.left   = upperLeft.x;
		ourRect.bottom = upperLeft.y + renderer.GetLineHeight(fontSize);
		ourRect.right  = upperLeft.x + renderer.GetGreekCharWidth(fontSize, kGreekPiChar);

		const JCoordinate ourMidline = ourRect.ycenter();
		return rectList->AddRect(ourRect, ourMidline, fontSize, this);
		}
	else
		{
		return JFunction::PrepareToRender(renderer, upperLeft, fontSize, rectList);
		}
}

/******************************************************************************
 Render

	Draw pi using the Greek symbol.

 ******************************************************************************/

void
JNamedConstant::Render
	(
	const JExprRenderer& renderer,
	const JExprRectList& rectList
	)
	const
{
	if (strcmp(JPGetStdNamedConstName(itsNameIndex), JPGetPiString()) == 0)
		{
		// find ourselves in the list

		JIndex ourIndex;
		const JBoolean found = rectList.FindFunction(this, &ourIndex);
		assert( found );

		const JRect ourRect = rectList.GetRect(ourIndex);
		const JCoordinate ourMidline = rectList.GetMidline(ourIndex);
		const JSize fontSize = rectList.GetFontSize(ourIndex);

		// draw the greek character for pi

		renderer.DrawGreekCharacter(ourRect.left, ourMidline, fontSize, kGreekPiChar);
		}
	else
		{
		JFunction::Render(renderer, rectList);
		}
}
