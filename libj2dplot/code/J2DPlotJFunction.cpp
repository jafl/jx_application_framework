/*********************************************************************************
 J2DPlotJFunction.cpp

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "J2DPlotJFunction.h"
#include <JExprParser.h>
#include <JVariableList.h>
#include <JFunction.h>
#include <JString.h>
#include <jAssert.h>

/*********************************************************************************
 Create

 ********************************************************************************/

JBoolean
J2DPlotJFunction::Create
	(
	J2DPlotJFunction**	plotfunction,
	J2DPlotWidget*		plot,
	JVariableList*		varList,
	JFontManager*		fontManager,
	const JString&		function,
	const JIndex		xIndex,
	const JFloat		xMin,
	const JFloat		xMax
	)
{
	JExprParser p(varList, fontManager);

	JFunction* f;
	if (p.Parse(function, &f))
		{
		*plotfunction = jnew J2DPlotJFunction(plot, varList, f, kJTrue, xIndex, xMin, xMax);
		return kJTrue;
		}
	else
		{
		*plotfunction = nullptr;
		return kJFalse;
		}
}

/*********************************************************************************
 Constructor

 ********************************************************************************/

J2DPlotJFunction::J2DPlotJFunction
	(
	J2DPlotWidget*	plot,
	JVariableList*	varList,
	JFunction*		f,
	const JBoolean	ownsFn,
	const JIndex	xIndex,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFunctionBase(kScatterPlot, plot, xMin, xMax)
{
	itsVarList    = varList;
	itsFunction   = f;
	itsOwnsFnFlag = ownsFn;
	itsXIndex     = xIndex;

	ListenTo(itsVarList);
}

/*********************************************************************************
 Destructor

 ********************************************************************************/

J2DPlotJFunction::~J2DPlotJFunction()
{
	if (itsOwnsFnFlag)
		{
		jdelete itsFunction;
		}
}

/*********************************************************************************
 GetYValue

 ********************************************************************************/

JBoolean
J2DPlotJFunction::GetYValue
	(
	const JFloat	x,
	JFloat*			y
	)
	const
{
	itsVarList->SetNumericValue(itsXIndex, 1, x);
	return itsFunction->Evaluate(y);
}

/*********************************************************************************
 GetFunctionString (virtual)

 ********************************************************************************/

JString
J2DPlotJFunction::GetFunctionString()
	const
{
	return itsFunction->Print();
}

/*********************************************************************************
 SetFunction

 ********************************************************************************/

void
J2DPlotJFunction::SetFunction
	(
	JVariableList*	varList,
	JFunction*		f,
	const JBoolean	ownsFn,
	const JIndex	xIndex,
	const JFloat	xMin,
	const JFloat	xMax
	)
{
	if (varList != itsVarList)
		{
		StopListening(itsVarList);
		itsVarList = varList;
		ListenTo(itsVarList);
		}

	if (itsOwnsFnFlag)
		{
		jdelete itsFunction;
		}

	itsFunction   = f;
	itsOwnsFnFlag = ownsFn;
	itsXIndex     = xIndex;

	SetXRange(xMin, xMax);

	UpdateFunction();
	BroadcastCurveChanged();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
J2DPlotJFunction::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsVarList && message.Is(JVariableList::kVarInserted))
		{
		const JVariableList::VarInserted* info =
			dynamic_cast<const JVariableList::VarInserted*>(&message);
		assert( info != nullptr );
		info->AdjustIndex(&itsXIndex);
		}

	else if (sender == itsVarList && message.Is(JVariableList::kVarRemoved))
		{
		const JVariableList::VarRemoved* info =
			dynamic_cast<const JVariableList::VarRemoved*>(&message);
		assert( info != nullptr );
		const JBoolean ok = info->AdjustIndex(&itsXIndex);
		assert( ok );	// client must ensure this
		}

	else if (sender == itsVarList && message.Is(JVariableList::kVarMoved))
		{
		const JVariableList::VarMoved* info =
			dynamic_cast<const JVariableList::VarMoved*>(&message);
		assert( info != nullptr );
		info->AdjustIndex(&itsXIndex);
		}

	else if (sender == itsVarList && message.Is(JVariableList::kVarValueChanged))
		{
		const JVariableList::VarValueChanged* info =
			dynamic_cast<const JVariableList::VarValueChanged*>(&message);
		assert( info != nullptr );
		if (info->GetVarIndex() != itsXIndex)
			{
			UpdateFunction();
			BroadcastCurveChanged();
			}
		}

	else
		{
		JPlotFunctionBase::Receive(sender, message);
		}
}
