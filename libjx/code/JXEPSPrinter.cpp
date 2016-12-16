/******************************************************************************
 JXEPSPrinter.cpp

	This class implements the functions required to draw to an
	Encapsulated Postscript file.

	BASE CLASS = JEPSPrinter

	Copyright (C) 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXEPSPrinter.h>
#include <JXEPSPrintSetupDialog.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <JXImagePainter.h>
#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JCharacter kSetupDataEndDelimiter = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

JXEPSPrinter::JXEPSPrinter
	(
	JXDisplay* display
	)
	:
	JEPSPrinter(display->GetFontManager(), display->GetColormap())
{
	itsDisplay = display;

	itsPreviewImage   = NULL;
	itsPreviewPainter = NULL;

	itsPrintSetupDialog = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEPSPrinter::~JXEPSPrinter()
{
	jdelete itsPreviewPainter;
	jdelete itsPreviewImage;
}

/******************************************************************************
 ReadXEPSSetup

	Automatically calls ReadEPSSetup().

 ******************************************************************************/

void
JXEPSPrinter::ReadXEPSSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	JIgnoreUntil(input, kSetupDataEndDelimiter);

	ReadEPSSetup(input);
}

/******************************************************************************
 WriteXEPSSetup

	Automatically calls WriteEPSSetup().

 ******************************************************************************/

void
JXEPSPrinter::WriteXEPSSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << kSetupDataEndDelimiter;

	WriteEPSSetup(output);

	output << ' ';
}

/******************************************************************************
 GetPreviewPainter (virtual)

 ******************************************************************************/

JPainter&
JXEPSPrinter::GetPreviewPainter
	(
	const JRect& bounds
	)
{
	itsPreviewImage =
		jnew JXImage(itsDisplay, bounds.width(), bounds.height(),
					(itsDisplay->GetColormap())->GetWhiteColor());
	assert( itsPreviewImage != NULL );

	itsPreviewPainter = itsPreviewImage->CreatePainter();
	itsPreviewPainter->SetOrigin(-bounds.left, -bounds.top);

	return *itsPreviewPainter;
}

/******************************************************************************
 GetPreviewImage (virtual protected)

 ******************************************************************************/

JBoolean
JXEPSPrinter::GetPreviewImage
	(
	const JImage** image
	)
	const
{
	if (itsPreviewImage != NULL)
		{
		*image = itsPreviewImage;
		return kJTrue;
		}
	else
		{
		*image = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 DeletePreviewData (virtual protected)

 ******************************************************************************/

void
JXEPSPrinter::DeletePreviewData()
{
	jdelete itsPreviewPainter;
	itsPreviewPainter = NULL;

	jdelete itsPreviewImage;
	itsPreviewImage = NULL;
}

/******************************************************************************
 BeginUserPrintSetup

	Displays a dialog with print setup information.  We broadcast
	PrintSetupFinished when the dialog is closed.

 ******************************************************************************/

void
JXEPSPrinter::BeginUserPrintSetup()
{
	assert( itsPrintSetupDialog == NULL );

	itsPrintSetupDialog =
		CreatePrintSetupDialog(GetOutputFileName(),
							   WantsPreview(), PSWillPrintBlackWhite());

	itsPrintSetupDialog->BeginDialog();
	ListenTo(itsPrintSetupDialog);
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXEPSPrintSetupDialog*
JXEPSPrinter::CreatePrintSetupDialog
	(
	const JCharacter*	fileName,
	const JBoolean		preview,
	const JBoolean		bw
	)
{
	return JXEPSPrintSetupDialog::Create(fileName, preview, bw);
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns kJTrue if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
JXEPSPrinter::EndUserPrintSetup
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
JXEPSPrinter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrintSetupDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		JBoolean changed = kJFalse;
		const JBoolean success = EndUserPrintSetup(message, &changed);
		Broadcast(PrintSetupFinished(success, changed));
		}

	else
		{
		JEPSPrinter::Receive(sender, message);
		}
}
