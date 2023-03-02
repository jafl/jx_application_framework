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
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

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
 ConfirmUserPrintSetup

	Displays a dialog with print setup information.  Returns true if the
	user confirms printing.

 ******************************************************************************/

bool
JXEPSPrinter::ConfirmUserPrintSetup()
{
	auto* dlog =
		CreatePrintSetupDialog(GetOutputFileName(),
							   WantsPreview(), PSWillPrintBlackWhite());

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

JXEPSPrintSetupDialog*
JXEPSPrinter::CreatePrintSetupDialog
	(
	const JString&	fileName,
	const bool		preview,
	const bool		bw
	)
{
	return JXEPSPrintSetupDialog::Create(fileName, preview, bw);
}
