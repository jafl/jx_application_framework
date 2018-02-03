/******************************************************************************
 JX2DCursorMarkTableDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <JX2DCursorMarkTableDir.h>
#include <JX2DCursorMarkTable.h>

#include <JXTextButton.h>
#include <JXScrollbarSet.h>
#include <JXWindow.h>
#include <JXScrollbar.h>

#include <J2DPlotWidget.h>
#include <jGlobals.h>
#include <jAssert.h>

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

	JXWindow* window = jnew JXWindow(this, 250,200, JString::empty);
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 250,200);
	assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle("Cursor Mark Values");
    window->SetMinSize(250,300);
    window->SetCloseAction(JXWindow::kDeactivateDirector);

	itsTable =
		jnew JX2DCursorMarkTable(plot,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert (itsTable != NULL);
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
		const JBoolean hasScrollbars = itsTable->GetScrollbars(&hScrollbar, &vScrollbar);
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
