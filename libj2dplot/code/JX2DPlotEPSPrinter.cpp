/******************************************************************************
 JX2DPlotEPSPrinter.cpp

	BASE CLASS = JXEPSPrinter

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JX2DPlotEPSPrinter.h"
#include <jx-af/jcore/JPSPrinter.h>
#include "J2DPlotWidget.h"
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JUtf8Byte kSetupDataEndDelimiter = '\1';

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

	itsUsePlotSetupFlag = true;
	itsPlotSetupDialog  = nullptr;
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
	std::istream& input
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
	std::ostream& output
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
	const JString&	fileName,
	const bool		preview,
	const bool		bw
	)
{
	if (itsUsePlotSetupFlag)
	{
		return JX2DPlotPrintEPSDialog::Create(fileName, preview, bw,
											  itsPlotWidth, itsPlotHeight, itsUnit);
	}
	else
	{
		return JXEPSPrinter::CreatePrintSetupDialog(fileName, preview, bw);
	}
}
