/******************************************************************************
 JXPTPrinter.cpp

	This class implements the functions required to print plain text.

	BASE CLASS = JPTPrinter

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXPTPrinter.h"
#include "JXPTPageSetupDialog.h"
#include "JXPTPrintSetupDialog.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JUtf8Byte kSetupDataEndDelimiter  = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPTPrinter::JXPTPrinter()
	:
	JPTPrinter(),
	itsDestination(kPrintToPrinter),
	itsPrintCmd("lpr")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPTPrinter::~JXPTPrinter()
{
}

/******************************************************************************
 SetDestination

	cmd and fileName can be empty.  They are there to force the caller to
	consider them.

 ******************************************************************************/

void
JXPTPrinter::SetDestination
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
JXPTPrinter::SetPrintCmd
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
JXPTPrinter::SetFileName
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
 ReadXPTSetup

	Automatically calls ReadPTSetup().

 ******************************************************************************/

void
JXPTPrinter::ReadXPTSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers <= kCurrentSetupVersion)
	{
		input >> itsDestination >> itsFileName;
		input >> itsPrintCmd;
	}
	JIgnoreUntil(input, kSetupDataEndDelimiter);

	ReadPTSetup(input);
}

/******************************************************************************
 WriteXPTSetup

	Automatically calls WritePTSetup().

 ******************************************************************************/

void
JXPTPrinter::WriteXPTSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsDestination;
	output << ' ' << itsFileName;
	output << ' ' << itsPrintCmd;
	output << kSetupDataEndDelimiter;

	WritePTSetup(output);

	output << ' ';
}

/******************************************************************************
 Print (virtual)

 ******************************************************************************/

void
JXPTPrinter::Print
	(
	const JString& text
	)
{
	JString fileName;
	if (itsDestination == kPrintToPrinter)
	{
		if (!JCreateTempFile(&fileName))
		{
			fileName = JGetRootDirectory();		// force failure below
		}
	}
	else
	{
		assert( itsDestination == kPrintToFile );
		fileName = itsFileName;
	}

	std::ofstream output(fileName.GetBytes());
	if (output.fail())
	{
		if (itsDestination == kPrintToPrinter)
		{
			JGetUserNotification()->ReportError(JGetString("CannotCreateTempFile::JXPTPrinter"));
		}
		else
		{
			assert( itsDestination == kPrintToFile );
			JGetUserNotification()->ReportError(JGetString("CannotCreateFile::JXPTPrinter"));
		}
		return;
	}

	bool success = JPTPrinter::Print(text, output);
	if (output.fail())
	{
		success = false;
		JGetUserNotification()->ReportError(JGetString("CannotPrint::JXPTPrinter"));
	}

	output.close();

	bool removeFile = false;
	if (success && itsDestination == kPrintToPrinter)
	{
		const JString sysCmd  = itsPrintCmd + " " + JPrepArgForExec(fileName);
		const JSize copyCount = GetCopyCount();
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

		removeFile = true;
	}
	else if (!success)
	{
		removeFile = true;
	}

	if (removeFile)
	{
		const bool ok = JRemoveFile(fileName);
		assert( ok );
	}
}

/******************************************************************************
 EditUserPageSetup

	Displays a dialog with page setup information.  Returns true if the
	setup changed.

 ******************************************************************************/

bool
JXPTPrinter::EditUserPageSetup()
{
	auto* dlog =
		CreatePageSetupDialog(itsPrintCmd, GetPageWidth(), GetPageHeight(),
							  GetHeaderLineCount() + GetFooterLineCount() + 1,
							  WillPrintReverseOrder());

	return dlog->DoDialog() && dlog->SetParameters(this);
}

/******************************************************************************
 CreatePageSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXPTPageSetupDialog*
JXPTPrinter::CreatePageSetupDialog
	(
	const JString&	printCmd,
	const JSize		pageWidth,
	const JSize		pageHeight,
	const JSize		minPageHeight,
	const bool		printReverseOrder
	)
{
	return JXPTPageSetupDialog::Create(printCmd, pageWidth, pageHeight,
									   minPageHeight, printReverseOrder);
}

/******************************************************************************
 ConfirmUserPrintSetup

	Displays a dialog with print setup information.  Returns true if the
	user confirms printing.

 ******************************************************************************/

bool
JXPTPrinter::ConfirmUserPrintSetup()
{
	auto* dlog =
		CreatePrintSetupDialog(itsDestination, itsPrintCmd, itsFileName,
							   WillPrintLineNumbers());

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

JXPTPrintSetupDialog*
JXPTPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JString&		printCmd,
	const JString&		fileName,
	const bool			printLineNumbers
	)
{
	return JXPTPrintSetupDialog::Create(destination, printCmd, fileName,
										printLineNumbers);
}

/******************************************************************************
 Global functions for JXPTPrinter

 ******************************************************************************/

/******************************************************************************
 Global functions for JXPTPrinter::Destination

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&					input,
	JXPTPrinter::Destination&	dest
	)
{
	long temp;
	input >> temp;
	dest = (JXPTPrinter::Destination) temp;
	assert( dest == JXPTPrinter::kPrintToPrinter ||
			dest == JXPTPrinter::kPrintToFile );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&						output,
	const JXPTPrinter::Destination	dest
	)
{
	output << (long) dest;
	return output;
}
