/******************************************************************************
 JXPTPrinter.cpp

	This class implements the functions required to draw to a Postscript file.

	BASE CLASS = JPTPrinter

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXPTPrinter.h>
#include <JXPTPageSetupDialog.h>
#include <JXPTPrintSetupDialog.h>
#include <JString.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jTime.h>
#include <jGlobals.h>
#include <jFStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

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
	itsPrintCmd("lpr", 0, kJFalse),
	itsPageSetupDialog(NULL),
	itsPrintSetupDialog(NULL)
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

	cmd and fileName can be NULL.  They are there to force the caller to
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

	if (printCmd != NULL)
		{
		itsPrintCmd = printCmd;
		}

	if (fileName != NULL)
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
		if (!(JCreateTempFile(&fileName)).OK())
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
			(JGetUserNotification())->ReportError(JGetString("CannotCreateTempFile::JXPTPrinter"));
			}
		else
			{
			assert( itsDestination == kPrintToFile );
			(JGetUserNotification())->ReportError(JGetString("CannotCreateFile::JXPTPrinter"));
			}
		return;
		}

	JBoolean success = JPTPrinter::Print(text, output);
	if (output.fail())
		{
		success = kJFalse;
		(JGetUserNotification())->ReportError(JGetString("CannotPrint::JXPTPrinter"));
		}

	output.close();

	JBoolean removeFile = kJFalse;
	if (success && itsDestination == kPrintToPrinter)
		{
		const JString sysCmd  = itsPrintCmd + " " + JPrepArgForExec(fileName);
		const JSize copyCount = GetCopyCount();
		for (JIndex i=1; i<=copyCount; i++)
			{
			const JError err = JExecute(sysCmd, NULL);
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

		removeFile = kJTrue;
		}
	else if (!success)
		{
		removeFile = kJTrue;
		}

	if (removeFile)
		{
		const JError err = JRemoveFile(fileName);
		assert_ok( err );
		}
}

/******************************************************************************
 BeginUserPageSetup

	Displays a dialog with page setup information.  We broadcast
	PageSetupFinished when the dialog is closed.

 ******************************************************************************/

void
JXPTPrinter::BeginUserPageSetup()
{
	assert( itsPageSetupDialog == NULL && itsPrintSetupDialog == NULL );

	itsPageSetupDialog =
		CreatePageSetupDialog(itsPrintCmd, GetPageWidth(), GetPageHeight(),
							  GetHeaderLineCount() + GetFooterLineCount() + 1,
							  WillPrintReverseOrder());

	itsPageSetupDialog->BeginDialog();
	ListenTo(itsPageSetupDialog);
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
	const JBoolean	printReverseOrder
	)
{
	return JXPTPageSetupDialog::Create(printCmd, pageWidth, pageHeight,
									   minPageHeight, printReverseOrder);
}

/******************************************************************************
 EndUserPageSetup (virtual protected)

	Returns kJTrue if settings were changed.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
JXPTPrinter::EndUserPageSetup
	(
	const JBroadcaster::Message& message
	)
{
	assert( itsPageSetupDialog != NULL );
	assert( message.Is(JXDialogDirector::kDeactivated) );

	const JXDialogDirector::Deactivated* info =
		dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
	assert( info != NULL );

	JBoolean changed = kJFalse;
	if (info->Successful())
		{
		changed = itsPageSetupDialog->SetParameters(this);
		}

	itsPageSetupDialog = NULL;
	return changed;
}

/******************************************************************************
 BeginUserPrintSetup

	Displays a dialog with print setup information.  We broadcast
	PrintSetupFinished when the dialog is closed.

 ******************************************************************************/

void
JXPTPrinter::BeginUserPrintSetup()
{
	assert( itsPageSetupDialog == NULL && itsPrintSetupDialog == NULL );

	itsPrintSetupDialog =
		CreatePrintSetupDialog(itsDestination, itsPrintCmd, itsFileName,
							   WillPrintLineNumbers());

	itsPrintSetupDialog->BeginDialog();
	ListenTo(itsPrintSetupDialog);
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
	const JBoolean		printLineNumbers
	)
{
	return JXPTPrintSetupDialog::Create(destination, printCmd, fileName,
										printLineNumbers);
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns kJTrue if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
JXPTPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message&	message,
	JBoolean*						changed
	)
{
	assert( itsPrintSetupDialog != NULL );
	assert( message.Is(JXDialogDirector::kDeactivated) );

	const JXDialogDirector::Deactivated* info =
		dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
	assert( info != NULL );

	if (info->Successful())
		{
		*changed = itsPrintSetupDialog->SetParameters(this);
		}

	itsPrintSetupDialog = NULL;
	return info->Successful();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXPTPrinter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPageSetupDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		Broadcast(PageSetupFinished(EndUserPageSetup(message)));
		}
	else if (sender == itsPrintSetupDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		JBoolean changed = kJFalse;
		const JBoolean success = EndUserPrintSetup(message, &changed);
		Broadcast(PrintSetupFinished(success, changed));
		}

	else
		{
		JPTPrinter::Receive(sender, message);
		}
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
