/******************************************************************************
 SCPlotDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCPlotDirector.h"
#include "SCCircuitDocument.h"
#include "SCCircuitVarList.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JX2DPlotWidget.h>

#include <J2DPlotJFunction.h>
#include <JFunction.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCPlotDirector::SCPlotDirector
	(
	SCCircuitDocument* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsDoc = supervisor;
	itsDoc->PlotCreated(this);

	BuildWindow();
}

SCPlotDirector::SCPlotDirector
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuitDocument*	supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsDoc = supervisor;
	itsDoc->PlotCreated(this);

	BuildWindow();
	(GetWindow())->ReadGeometry(input);

	JSize fnCount;
	input >> fnCount;

	for (JIndex i=1; i<=fnCount; i++)
		{
		JFloat xMin, xMax;
		input >> xMin >> xMax;

		JFunction* f = JFunction::StreamIn(input, itsDoc->GetVarList());
		AddFunction(f, "", xMin, xMax);
		}

	itsPlot->PWReadSetup(input);
	itsPlot->PWReadCurveSetup(input);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCPlotDirector::~SCPlotDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SCPlotDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 600,400, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( menuBar != NULL );

	itsPlot =
		new JX2DPlotWidget(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,370);
	assert( itsPlot != NULL );

// end JXLayout

	window->LockCurrentMinSize();

	ListenTo(itsPlot);
	itsPlot->SetTitle("New plot");
	itsPlot->SetXLabel("Frequency (Hz)");
}

/******************************************************************************
 AddFunction

 ******************************************************************************/

void
SCPlotDirector::AddFunction
	(
	const JFunction&	f,
	const JCharacter*	name,
	const JFloat		xMin,
	const JFloat		xMax
	)
{
	AddFunction(f.Copy(), name, xMin, xMax);
}

/******************************************************************************
 AddFunction (private)

	We take ownership of the function.

 ******************************************************************************/

void
SCPlotDirector::AddFunction
	(
	JFunction*			f,
	const JCharacter*	name,
	const JFloat		xMin,
	const JFloat		xMax
	)
{
	SCCircuitVarList* varList = const_cast<SCCircuitVarList*>(itsDoc->GetVarList());

	J2DPlotJFunction* data =
		new J2DPlotJFunction(itsPlot, varList, f, kJTrue,
							 varList->GetFrequencyVarIndex(), xMin, xMax);
	assert( data != NULL );

	itsPlot->AddCurve(data, kJTrue, name, kJTrue, kJFalse);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SCPlotDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlot && message.Is(J2DPlotWidget::kTitleChanged))
		{
		(GetWindow())->SetTitle(itsPlot->GetTitle());
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCPlotDirector::StreamOut
	(
	ostream& output
	)
	const
{
	output << ' ';
	(GetWindow())->WriteGeometry(output);

	const JSize fnCount = itsPlot->GetCurveCount();
	output << ' ' << fnCount;

	for (JIndex i=1; i<=fnCount; i++)
		{
		const JPlotDataBase& data      = itsPlot->GetCurve(i);
		const J2DPlotJFunction* fnData = dynamic_cast<const J2DPlotJFunction*>(&data);
		assert( fnData != NULL );

		JFloat xMin, xMax;
		fnData->GetXRange(&xMin, &xMax);
		output << ' ' << xMin << ' ' << xMax;

		output << ' ';
		(fnData->GetFunction()).StreamOut(output);
		}

	output << ' ';
	itsPlot->PWWriteSetup(output);

	output << ' ';
	itsPlot->PWWriteCurveSetup(output);

	output << ' ';
}
