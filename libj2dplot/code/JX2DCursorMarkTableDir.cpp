/******************************************************************************
 JX2DCursorMarkTableDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "jx-af/j2dplot/JX2DCursorMarkTableDir.h"
#include "jx-af/j2dplot/JX2DCursorMarkTable.h"

#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbar.h>

#include "jx-af/j2dplot/J2DPlotWidget.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 1;	// must begin with digit >= 1

	// version 1:  added version number

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DCursorMarkTableDir::JX2DCursorMarkTableDir
	(
	JXWindowDirector*	supervisor,
	J2DPlotWidget*		plot
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow(plot);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DCursorMarkTableDir::~JX2DCursorMarkTableDir()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DCursorMarkTableDir::BuildWindow
	(
	J2DPlotWidget* plot
	)
{
// begin JXLayout

	const auto* window = jnew JXWindow(this, 250,200, JString::empty);
	assert( window != nullptr );

	const auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 250,200);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JX2DCursorMarkTableDir"));
	window->SetMinSize(250,300);
	window->SetCloseAction(JXWindow::kDeactivateDirector);

	itsTable =
		jnew JX2DCursorMarkTable(plot,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert (itsTable != nullptr);
	itsTable->FitToEnclosure();
}

/******************************************************************************
 ReadSetup

	This uses assert() to check the version number because we cannot use
	a marker to skip the data (since it contains objects), so it is the
	client's responsibility to check whether or not the file can be
	read (via a higher level version number).

 ******************************************************************************/

void
JX2DCursorMarkTableDir::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers = 0;
	input >> std::ws;
	if (input.peek() != '0')	// version 0 => leave it for JXWindow
	{
		input >> vers;
		assert( vers <= kCurrentSetupVersion );
	}

	GetWindow()->ReadGeometry(input);

	itsTable->AdjustTable();

	if (vers == 0)
	{
		JXScrollbar *hScrollbar, *vScrollbar;
		const bool hasScrollbars = itsTable->GetScrollbars(&hScrollbar, &vScrollbar);
		assert( hasScrollbars );

		vScrollbar->ReadSetup(input);
		hScrollbar->ReadSetup(input);
	}
	else
	{
		itsTable->ReadScrollSetup(input);
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JX2DCursorMarkTableDir::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	// must be last so the table can adjust before the scrollbars are adjusted

	output << ' ';
	itsTable->WriteScrollSetup(output);

	output << ' ';
}
