/******************************************************************************
 JXEPSPrinter.cpp

	This class implements the functions required to draw to an
	Encapsulated Postscript file.

	BASE CLASS = JEPSPrinter

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#include "JXEPSPrinter.h"
#include "JXEPSPrintSetupDialog.h"
#include "JXDisplay.h"
#include "JXImage.h"
#include "JXImagePainter.h"
#include <JColorManager.h>
#include <jStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JUtf8Byte kSetupDataEndDelimiter  = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

JXEPSPrinter::JXEPSPrinter
	(
	JXDisplay* display
	)
	:
	JEPSPrinter(display->GetFontManager())
{
	itsDisplay = display;

	itsPreviewImage   = nullptr;
	itsPreviewPainter = nullptr;

	itsPrintSetupDialog = nullptr;
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
					 JColorManager::GetWhiteColor());
	assert( itsPreviewImage != nullptr );

	itsPreviewPainter = itsPreviewImage->CreatePainter();
	itsPreviewPainter->SetOrigin(-bounds.left, -bounds.top);

	return *itsPreviewPainter;
}

/******************************************************************************
 GetPreviewImage (virtual protected)

 ******************************************************************************/

bool
JXEPSPrinter::GetPreviewImage
	(
	const JImage** image
	)
	const
{
	if (itsPreviewImage != nullptr)
		{
		*image = itsPreviewImage;
		return true;
		}
	else
		{
		*image = nullptr;
		return false;
		}
}

/******************************************************************************
 DeletePreviewData (virtual protected)

 ******************************************************************************/

void
JXEPSPrinter::DeletePreviewData()
{
	jdelete itsPreviewPainter;
	itsPreviewPainter = nullptr;

	jdelete itsPreviewImage;
	itsPreviewImage = nullptr;
}

/******************************************************************************
 BeginUserPrintSetup

	Displays a dialog with print setup information.  We broadcast
	PrintSetupFinished when the dialog is closed.

 ******************************************************************************/

void
JXEPSPrinter::BeginUserPrintSetup()
{
	assert( itsPrintSetupDialog == nullptr );

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
	const JString&	fileName,
	const bool	preview,
	const bool	bw
	)
{
	return JXEPSPrintSetupDialog::Create(fileName, preview, bw);
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns true if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

bool
JXEPSPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message&	message,
	bool*						changed
	)
{
	assert( itsPrintSetupDialog != nullptr );
	assert( message.Is(JXDialogDirector::kDeactivated) );

	const auto* info =
		dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
	assert( info != nullptr );

	if (info->Successful())
		{
		*changed = itsPrintSetupDialog->SetParameters(this);
		}

	itsPrintSetupDialog = nullptr;
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
		bool changed = false;
		const bool success = EndUserPrintSetup(message, &changed);
		Broadcast(PrintSetupFinished(success, changed));
		}

	else
		{
		JEPSPrinter::Receive(sender, message);
		}
}
