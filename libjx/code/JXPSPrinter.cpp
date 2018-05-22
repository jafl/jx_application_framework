/******************************************************************************
 JXPSPrinter.cpp

	This class implements the functions required to draw to a Postscript file.

	BASE CLASS = JPSPrinter

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include <JXPSPrinter.h>
#include <JXPSPageSetupDialog.h>
#include <JXPSPrintSetupDialog.h>
#include <JXDisplay.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jTime.h>
#include <stdio.h>
#include <jAssert.h>

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
	JPSPrinter(display->GetFontManager(), display->GetColorManager()),
	itsPrintCmd("lpr", 0)
{
	itsDestination = kPrintToPrinter;
	itsCollateFlag = kJFalse;

	itsPageSetupDialog  = NULL;
	itsPrintSetupDialog = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPSPrinter::~JXPSPrinter()
{
}

/******************************************************************************
 SetDestination

	cmd and fileName can be NULL.  They are there to force the caller to
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
		JBoolean printBW;
		input >> type >> orient >> itsPrintCmd >> printBW;
		SetPaperType(type);
		SetOrientation(orient);
		PSPrintBlackWhite(printBW);
		itsCollateFlag = kJFalse;
		PSResetCoordinates();
		}
	else if (vers == 1)
		{
		input >> itsPrintCmd;
		itsCollateFlag = kJFalse;
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
			input >> itsPrintCmd >> itsCollateFlag;
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
	output << ' ' << itsCollateFlag;
	output << kSetupDataEndDelimiter;

	WritePSSetup(output);

	output << ' ';
}

/******************************************************************************
 OpenDocument (virtual)

 ******************************************************************************/

JBoolean
JXPSPrinter::OpenDocument()
{
	const JSize savedCopyCount = GetCopyCount();
	if (itsDestination == kPrintToPrinter)
		{
		JString outputFile;
		if (!(JCreateTempFile(&outputFile)).OK())
			{
			return kJFalse;
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

	const JBoolean success = JPSPrinter::OpenDocument();

	if (itsDestination == kPrintToPrinter && itsCollateFlag)
		{
		SetCopyCount(savedCopyCount);
		}

	if (!success)
		{
		if (itsDestination == kPrintToPrinter)
			{
			(JGetUserNotification())->ReportError(JGetString("UnableToCreateTempFile::JXPSPrinter"));
			}
		else
			{
			assert( itsDestination == kPrintToFile );
			(JGetUserNotification())->ReportError(JGetString("UnableToSave::JXPSPrinter"));
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

		JRemoveFile(fileName);
		}
}

/******************************************************************************
 BeginUserPageSetup

	Displays a dialog with print setup information.  We broadcast
	PageSetupFinished when the dialog is closed.

 ******************************************************************************/

void
JXPSPrinter::BeginUserPageSetup()
{
	assert( itsPageSetupDialog == NULL && itsPrintSetupDialog == NULL );

	itsPageSetupDialog =
		CreatePageSetupDialog(GetPaperType(), GetOrientation());

	itsPageSetupDialog->BeginDialog();
	ListenTo(itsPageSetupDialog);
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
 EndUserPageSetup (virtual protected)

	Returns kJTrue if settings were changed.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
JXPSPrinter::EndUserPageSetup
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
JXPSPrinter::BeginUserPrintSetup()
{
	assert( itsPageSetupDialog == NULL && itsPrintSetupDialog == NULL );

	itsPrintSetupDialog =
		CreatePrintSetupDialog(itsDestination, itsPrintCmd,
							   itsFileName, itsCollateFlag,
							   PSWillPrintBlackWhite());

	itsPrintSetupDialog->BeginDialog();
	ListenTo(itsPrintSetupDialog);
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
	const JBoolean		collate,
	const JBoolean		bw
	)
{
	return JXPSPrintSetupDialog::Create(destination, printCmd, fileName, collate, bw);
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns kJTrue if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
JXPSPrinter::EndUserPrintSetup
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
JXPSPrinter::Receive
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
		JPSPrinter::Receive(sender, message);
		}
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
