/******************************************************************************
 JXPSPrinter.cpp

	This class implements the functions required to draw to a Postscript file.

	BASE CLASS = JPSPrinter

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXPSPrinter.h"
#include "JXPSPageSetupDialog.h"
#include "JXPSPrintSetupDialog.h"
#include "JXDisplay.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jTime.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 3;
const JUtf8Byte kSetupDataEndDelimiter  = '\1';
// version  1 split data between JPSPrinter and JXPSPrinter
// version  2 added collating flag and ending delimiter
// version  3 added destination and file name

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPSPrinter::JXPSPrinter
	(
	const JXDisplay* display
	)
	:
	JPSPrinter(display->GetFontManager()),
	itsDestination(kPrintToPrinter),
	itsPrintCmd("lpr"),
	itsCollateFlag(false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPSPrinter::~JXPSPrinter()
{
}

/******************************************************************************
 SetDestination

	cmd and fileName can be empty.  They are there to force the caller to
	consider them.

 ******************************************************************************/

void
JXPSPrinter::SetDestination
	(
	const Destination	dest,
	const JString&		printCmd,
	const JString&		fileName
	)
{
	itsDestination = dest;

	if (!printCmd.IsEmpty())
	{
		itsPrintCmd = printCmd;
	}

	if (!fileName.IsEmpty())
	{
		itsFileName = fileName;
	}
}

/******************************************************************************
 SetPrintCmd

 ******************************************************************************/

void
JXPSPrinter::SetPrintCmd
	(
	const JString& cmd
	)
{
	itsPrintCmd = cmd;
}

/******************************************************************************
 SetFileName

 ******************************************************************************/

void
JXPSPrinter::SetFileName
	(
	const JString& name
	)
{
	itsFileName = name;
	if (itsFileName.IsEmpty())
	{
		itsDestination = kPrintToPrinter;
	}
}

/******************************************************************************
 ReadXPSSetup

	Automatically calls ReadPSSetup().

 ******************************************************************************/

void
JXPSPrinter::ReadXPSSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers == 0)
	{
		PaperType type;
		ImageOrientation orient;
		bool printBW;
		input >> type >> orient >> itsPrintCmd >> JBoolFromString(printBW);
		SetPaperType(type);
		SetOrientation(orient);
		PSPrintBlackWhite(printBW);
		itsCollateFlag = false;
		PSResetCoordinates();
	}
	else if (vers == 1)
	{
		input >> itsPrintCmd;
		itsCollateFlag = false;
		ReadPSSetup(input);
	}
	else
	{
		if (vers <= kCurrentSetupVersion)
		{
			if (vers >= 3)
			{
				input >> itsDestination >> itsFileName;
			}
			input >> itsPrintCmd >> JBoolFromString(itsCollateFlag);
		}
		JIgnoreUntil(input, kSetupDataEndDelimiter);

		ReadPSSetup(input);
	}
}

/******************************************************************************
 WriteXPSSetup

	Automatically calls WritePSSetup().

 ******************************************************************************/

void
JXPSPrinter::WriteXPSSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsDestination;
	output << ' ' << itsFileName;
	output << ' ' << itsPrintCmd;
	output << ' ' << JBoolToString(itsCollateFlag);
	output << kSetupDataEndDelimiter;

	WritePSSetup(output);

	output << ' ';
}

/******************************************************************************
 OpenDocument (virtual)

 ******************************************************************************/

bool
JXPSPrinter::OpenDocument()
{
	const JSize savedCopyCount = GetCopyCount();
	if (itsDestination == kPrintToPrinter)
	{
		JString outputFile;
		if (!(JCreateTempFile(&outputFile)).OK())
		{
			return false;
		}

		SetOutputFileName(outputFile);

		if (itsCollateFlag)
		{
			SetCopyCount(1);
		}
	}
	else
	{
		assert( itsDestination == kPrintToFile );
		SetOutputFileName(itsFileName);
	}

	const bool success = JPSPrinter::OpenDocument();

	if (itsDestination == kPrintToPrinter && itsCollateFlag)
	{
		SetCopyCount(savedCopyCount);
	}

	if (!success)
	{
		if (itsDestination == kPrintToPrinter)
		{
			JGetUserNotification()->ReportError(JGetString("UnableToCreateTempFile::JXPSPrinter"));
		}
		else
		{
			assert( itsDestination == kPrintToFile );
			JGetUserNotification()->ReportError(JGetString("UnableToSave::JXPSPrinter"));
		}
	}

	return success;
}

/******************************************************************************
 CloseDocument (virtual)

 ******************************************************************************/

void
JXPSPrinter::CloseDocument()
{
	JPSPrinter::CloseDocument();

	if (itsDestination == kPrintToPrinter)
	{
		const JString& fileName = GetOutputFileName();
		const JString sysCmd    = itsPrintCmd + " " + JPrepArgForExec(fileName);

		const JSize copyCount = (itsCollateFlag ? GetCopyCount() : 1);

		for (JIndex i=1; i<=copyCount; i++)
		{
			const JError err = JExecute(sysCmd, nullptr);
			if (!err.OK())
			{
				err.ReportIfError();
				break;
			}

			if (i < copyCount)
			{
				JWait(2);
			}
		}

		JRemoveFile(fileName);
	}
}

/******************************************************************************
 EditUserPageSetup

	Displays a dialog with print setup information.  Returns true if the
	setup changed.

 ******************************************************************************/

bool
JXPSPrinter::EditUserPageSetup()
{
	auto* dlog = CreatePageSetupDialog(GetPaperType(), GetOrientation());

	return dlog->DoDialog() && dlog->SetParameters(this);
}

/******************************************************************************
 CreatePageSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXPSPageSetupDialog*
JXPSPrinter::CreatePageSetupDialog
	(
	const PaperType			paper,
	const ImageOrientation	orient
	)
{
	return JXPSPageSetupDialog::Create(paper, orient);
}

/******************************************************************************
 ConfirmUserPrintSetup

	Displays a dialog with print setup information.  Returns true if the
	user confirms printing.

 ******************************************************************************/

bool
JXPSPrinter::ConfirmUserPrintSetup()
{
	auto* dlog =
		CreatePrintSetupDialog(itsDestination, itsPrintCmd,
							   itsFileName, itsCollateFlag,
							   PSWillPrintBlackWhite());

	const bool ok = dlog->DoDialog();
	if (ok)
	{
		dlog->SetParameters(this);
	}
	return ok;
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXPSPrintSetupDialog*
JXPSPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JString&		printCmd,
	const JString&		fileName,
	const bool		collate,
	const bool		bw
	)
{
	return JXPSPrintSetupDialog::Create(destination, printCmd, fileName, collate, bw);
}

/******************************************************************************
 Global functions for JXPSPrinter

 ******************************************************************************/

/******************************************************************************
 Global functions for JXPSPrinter::Destination

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&					input,
	JXPSPrinter::Destination&	dest
	)
{
	long temp;
	input >> temp;
	dest = (JXPSPrinter::Destination) temp;
	assert( dest == JXPSPrinter::kPrintToPrinter ||
			dest == JXPSPrinter::kPrintToFile );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&						output,
	const JXPSPrinter::Destination	dest
	)
{
	output << (long) dest;
	return output;
}
