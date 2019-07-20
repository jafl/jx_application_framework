/******************************************************************************
 GDBPlot2DCommand.cpp

	BASE CLASS = CMPlot2DCommand

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "GDBPlot2DCommand.h"
#include "CMPlot2DDir.h"
#include "cmGlobals.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBPlot2DCommand::GDBPlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
	:
	CMPlot2DCommand(dir, x, y)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBPlot2DCommand::~GDBPlot2DCommand()
{
}

/******************************************************************************
 UpdateRange (virtual)

 ******************************************************************************/

void
GDBPlot2DCommand::UpdateRange
	(
	const JIndex	curveIndex,
	const JInteger	min,
	const JInteger	max
	)
{
	CMPlot2DCommand::UpdateRange(curveIndex, min, max);

	JString cmd("set print pretty off\nset print array off\n"
				"set print repeats 0\nset width 0\n");

	for (JInteger i=min; i<=max; i++)
		{
		cmd += "print ";
		cmd += GetDirector()->GetXExpression(curveIndex, i);
		cmd += "\n";

		cmd += "print ";
		cmd += GetDirector()->GetYExpression(curveIndex, i);
		cmd += "\n";
		}

	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern = "^\\$[[:digit:]]+[[:space:]]+=[[:space:]]+(.*)$";

void
GDBPlot2DCommand::HandleSuccess
	(
	const JString& data
	)
{
	JArray<JFloat>* x = GetX();
	JArray<JFloat>* y = GetY();

	if (GetLastResult().BeginsWith("error,msg=\"No symbol"))
		{
		x->RemoveAll();
		y->RemoveAll();
		return;
		}

	const JIndex count = x->GetElementCount();

	JIndex i;
	JString v1, v2;

	JStringIterator iter(data);
	for (i=1; i<=count; i++)
		{
		if (!iter.Next(prefixPattern))
			{
			break;
			}
		const JStringMatch m1 = iter.GetLastMatch();

		if (!iter.Next(prefixPattern))
			{
			break;
			}
		const JStringMatch m2 = iter.GetLastMatch();

		v1 = m1.GetSubstring(1);
		v1.TrimWhitespace();

		v2 = m2.GetSubstring(1);
		v2.TrimWhitespace();

		JFloat x1, y1;
		if (!v1.ConvertToFloat(&x1) ||
			!v2.ConvertToFloat(&y1))
			{
			break;
			}

		x->SetElement(i, x1);
		y->SetElement(i, y1);
		}

	if (i <= count)
		{
		const JSize delta = count - (i-1);
		x->RemoveNextElements(count - delta + 1, delta);
		y->RemoveNextElements(count - delta + 1, delta);
		}
}
