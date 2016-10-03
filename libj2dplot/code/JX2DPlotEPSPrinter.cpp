/******************************************************************************
 JX2DPlotEPSPrinter.cpp

	BASE CLASS = JXEPSPrinter

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JX2DPlotEPSPrinter.h>
#include <JPSPrinter.h>
#include <J2DPlotWidget.h>
#include <jStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JCharacter kSetupDataEndDelimiter = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotEPSPrinter::JX2DPlotEPSPrinter
	(
	JXDisplay* display
	)
	:
	JXEPSPrinter(display)
{
	itsPlotWidth  = JPSPrinter::GetPaperWidth(JPSPrinter::kUSLetter);
	itsPlotHeight = JPSPrinter::GetPaperHeight(JPSPrinter::kUSLetter);
	J2DPlotWidget::GetPSPortraitPrintSize( itsPlotWidth,  itsPlotHeight,
										  &itsPlotWidth, &itsPlotHeight);

	itsUnit = JX2DPlotPrintEPSDialog::kCentimeters;

	itsUsePlotSetupFlag = kJTrue;
	itsPlotSetupDialog  = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DPlotEPSPrinter::~JX2DPlotEPSPrinter()
{
}

/******************************************************************************
 ReadX2DEPSSetup

	Automatically calls ReadXEPSSetup().

 ******************************************************************************/

void
JX2DPlotEPSPrinter::ReadX2DEPSSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers <= kCurrentSetupVersion)
		{
		input >> itsPlotWidth >> itsPlotHeight >> itsUnit;
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);

	ReadXEPSSetup(input);
}

/******************************************************************************
 WriteX2DEPSSetup

	Automatically calls WriteXEPSSetup().

 ******************************************************************************/

void
JX2DPlotEPSPrinter::WriteX2DEPSSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsPlotWidth;
	output << ' ' << itsPlotHeight;
	output << ' ' << itsUnit;
	output << kSetupDataEndDelimiter;

	WriteXEPSSetup(output);
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXEPSPrintSetupDialog*
JX2DPlotEPSPrinter::CreatePrintSetupDialog
	(
	const JCharacter*	fileName,
	const JBoolean		preview,
	const JBoolean		bw
	)
{
	assert( itsPlotSetupDialog == NULL );

	if (itsUsePlotSetupFlag)
		{
		itsPlotSetupDialog =
			JX2DPlotPrintEPSDialog::Create(fileName, preview, bw,
										   itsPlotWidth, itsPlotHeight, itsUnit);
		return itsPlotSetupDialog;
		}
	else
		{
		return JXEPSPrinter::CreatePrintSetupDialog(fileName, preview, bw);
		}
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns kJTrue if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
JX2DPlotEPSPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message&	message,
	JBoolean*						changed
	)
{
	const JBoolean ok = JXEPSPrinter::EndUserPrintSetup(message, changed);
	if (itsPlotSetupDialog != NULL)
		{
		JCoordinate w,h;
		JX2DPlotPrintEPSDialog::Unit u;
		itsPlotSetupDialog->GetPlotSize(&w, &h, &u);

		*changed = JI2B(*changed ||
			w != itsPlotWidth  ||
			h != itsPlotHeight ||
			u != itsUnit);

		itsPlotWidth  = w;
		itsPlotHeight = h;
		itsUnit       = u;
		}

	itsPlotSetupDialog = NULL;
	return ok;
}
