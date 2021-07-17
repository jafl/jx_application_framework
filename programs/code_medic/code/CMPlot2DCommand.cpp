/******************************************************************************
 CMPlot2DCommand.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMPlot2DCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMPlot2DCommand::CMPlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
	:
	CMCommand("", false, true),
	itsDirector(dir),
	itsX(x),
	itsY(y)
{
	assert( itsX->GetElementCount() == itsY->GetElementCount() );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMPlot2DCommand::~CMPlot2DCommand()
{
}

/******************************************************************************
 UpdateRange (virtual)

 ******************************************************************************/

void
CMPlot2DCommand::UpdateRange
	(
	const JIndex	curveIndex,
	const JInteger	min,
	const JInteger	max
	)
{
	const JSize oldCount = itsX->GetElementCount();
	const JSize newCount = max - min + 1;

	if (newCount < oldCount)
		{
		const JSize delta = oldCount - newCount;
		itsX->RemoveNextElements(oldCount - delta + 1, delta);
		itsY->RemoveNextElements(oldCount - delta + 1, delta);
		}
	else if (oldCount < newCount)
		{
		for (JIndex i=oldCount+1; i<=newCount; i++)
			{
			itsX->AppendElement(0);
			itsY->AppendElement(0);
			}
		}
}
