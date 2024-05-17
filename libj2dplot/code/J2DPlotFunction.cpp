/*********************************************************************************
 J2DPlotFunction.cpp

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "J2DPlotFunction.h"
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JVariableList.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Create

 ********************************************************************************/

bool
J2DPlotFunction::Create
	(
	J2DPlotFunction**	plotfunction,
	J2DPlotWidget*		plot,
	JVariableList*		varList,
	const JString&		function,
	const JIndex		xIndex,
	const JFloat		xMin,
	const JFloat		xMax
	)
{
	JExprParser p(varList);

	JFunction* f;
	if (p.Parse(function, &f))
	{
		*plotfunction = jnew J2DPlotFunction(plot, varList, f, true, xIndex, xMin, xMax);
		return true;
	}
	else
	{
		*plotfunction = nullptr;
		return false;
	}
}

/*********************************************************************************
 Constructor

 ********************************************************************************/

J2DPlotFunction::J2DPlotFunction
	(
	J2DPlotWidget*	plot,
	JVariableList*	varList,
	JFunction*		f,
	const bool		ownsFn,
	const JIndex	xIndex,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	J2DPlotFunctionBase(kScatterPlot, plot, xMin, xMax)
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

J2DPlotFunction::~J2DPlotFunction()
{
	if (itsOwnsFnFlag)
	{
		jdelete itsFunction;
	}
}

/*********************************************************************************
 GetYValue

 ********************************************************************************/

bool
J2DPlotFunction::GetYValue
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
J2DPlotFunction::GetFunctionString()
	const
{
	return itsFunction->Print();
}

/*********************************************************************************
 SetFunction

 ********************************************************************************/

void
J2DPlotFunction::SetFunction
	(
	JVariableList*	varList,
	JFunction*		f,
	const bool	ownsFn,
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
J2DPlotFunction::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsVarList && message.Is(JVariableList::kVarInserted))
	{
		auto& info = dynamic_cast<const JVariableList::VarInserted&>(message);
		info.AdjustIndex(&itsXIndex);
	}

	else if (sender == itsVarList && message.Is(JVariableList::kVarRemoved))
	{
		auto& info    = dynamic_cast<const JVariableList::VarRemoved&>(message);
		const bool ok = info.AdjustIndex(&itsXIndex);
		assert( ok );	// client must ensure this
	}

	else if (sender == itsVarList && message.Is(JVariableList::kVarMoved))
	{
		auto& info = dynamic_cast<const JVariableList::VarMoved&>(message);
		info.AdjustIndex(&itsXIndex);
	}

	else if (sender == itsVarList && message.Is(JVariableList::kVarValueChanged))
	{
		auto& info = dynamic_cast<const JVariableList::VarValueChanged&>(message);
		if (info.GetVarIndex() != itsXIndex)
		{
			UpdateFunction();
			BroadcastCurveChanged();
		}
	}

	else
	{
		J2DPlotFunctionBase::Receive(sender, message);
	}
}
