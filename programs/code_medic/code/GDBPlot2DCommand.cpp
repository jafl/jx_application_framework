/******************************************************************************
 GDBPlot2DCommand.cpp

	BASE CLASS = CMPlot2DCommand

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "GDBPlot2DCommand.h"
#include "CMPlot2DDir.h"
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

	JString cmd = "set print pretty off\nset print array off\n"
				  "set print repeats 0\nset width 0\n";

	for (JInteger i=min; i<=max; i++)
		{
		cmd += "print ";
		cmd += (GetDirector())->GetXExpression(curveIndex, i);
		cmd.AppendCharacter('\n');

		cmd += "print ";
		cmd += (GetDirector())->GetYExpression(curveIndex, i);
		cmd.AppendCharacter('\n');
		}

	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

#include "cmGlobals.h"
static const JRegex prefixPattern = "^\\$[[:digit:]]+[[:space:]]+=[[:space:]]+(.*)$";

void
GDBPlot2DCommand::HandleSuccess
	(
	const JString& data
	)
{
	JArray<JFloat>* x = GetX();
	JArray<JFloat>* y = GetY();

	if ((GetLastResult()).BeginsWith("error,msg=\"No symbol"))
		{
		x->RemoveAll();
		y->RemoveAll();
		return;
		}

	const JIndex count = x->GetElementCount();

	JIndex i;
	JIndexRange r;
	JArray<JIndexRange> matchRange1, matchRange2;
	JString v1, v2;
	for (i=1; i<=count; i++)
		{
		if (!prefixPattern.MatchAfter(data, r, &matchRange1))
			{
			break;
			}
		r = matchRange1.GetElement(1);

		if (!prefixPattern.MatchAfter(data, r, &matchRange2))
			{
			break;
			}
		r = matchRange2.GetElement(1);

		v1 = data.GetSubstring(matchRange1.GetElement(2));
		v1.TrimWhitespace();

		v2 = data.GetSubstring(matchRange2.GetElement(2));
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
