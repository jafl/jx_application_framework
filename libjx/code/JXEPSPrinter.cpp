/******************************************************************************
 JXEPSPrinter.cpp

	This class implements the functions required to draw to an
	Encapsulated Postscript file.

	BASE CLASS = JEPSPrinter

	Copyright © 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
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
	JXDisplay*	display,
	JXColormap*	colormap
	)
	:
	JEPSPrinter(display->GetFontManager(), colormap)
{
	itsDisplay  = display;
	itsColormap = colormap;

	itsPreviewImage   = NULL;
	itsPreviewPainter = NULL;

	itsPrintSetupDialog = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEPSPrinter::~JXEPSPrinter()
{
	delete itsPreviewPainter;
	delete itsPreviewImage;
}

/******************************************************************************
 ReadXEPSSetup

	Automatically calls ReadEPSSetup().

 ******************************************************************************/

void
JXEPSPrinter::ReadXEPSSetup
	(
	istream& input
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
	ostream& output
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
		new JXImage(itsDisplay, itsColormap, bounds.width(), bounds.height(),
					itsColormap->GetWhiteColor());
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
	delete itsPreviewPainter;
	itsPreviewPainter = NULL;

	delete itsPreviewImage;
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
		dynamic_cast(const JXDialogDirector::Deactivated*, &message);
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
