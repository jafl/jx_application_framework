/******************************************************************************
 CBPSPrinter.cpp

	BASE CLASS = JXPSPrinter, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBPSPrinter.h"
#include "CBPSPrintSetupDialog.h"
#include "CBPTPrinter.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

const JSize kUnsetFontSize = 0;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsFontSize

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPSPrinter::CBPSPrinter
	(
	JXDisplay* display
	)
	:
	JXPSPrinter(display),
	JPrefObject(CBGetPrefsManager(), kCBPrintStyledTextID)
{
	itsFontSize           = kUnsetFontSize;
	itsTE                 = nullptr;
	itsCBPrintSetupDialog = nullptr;

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPSPrinter::~CBPSPrinter()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

static const JRegex nxmRegex("[0-9]+x([0-9]+)");

JXPSPrintSetupDialog*
CBPSPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JString&		printCmd,
	const JString&		fileName,
	const bool		collate,
	const bool		bw
	)
{
	assert( itsCBPrintSetupDialog == nullptr );

	if (itsFontSize == kUnsetFontSize)
		{
		JString fontName;
		CBGetPrefsManager()->GetDefaultFont(&fontName, &itsFontSize);

		const JStringMatch m = nxmRegex.Match(fontName, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
			{
			const JString hStr = m.GetSubstring(1);
			const bool ok  = hStr.ConvertToUInt(&itsFontSize);
			assert( ok );
			itsFontSize--;
			}
		}

	itsCBPrintSetupDialog =
		CBPSPrintSetupDialog::Create(destination, printCmd, fileName,
									 collate, bw, itsFontSize,
									 CBGetPTTextPrinter()->WillPrintHeader());
	return itsCBPrintSetupDialog;
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns true if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

bool
CBPSPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message&	message,
	bool*						changed
	)
{
	assert( itsCBPrintSetupDialog != nullptr );

	const bool ok = JXPSPrinter::EndUserPrintSetup(message, changed);
	if (ok)
		{
		JSize fontSize;
		bool printHeader;
		itsCBPrintSetupDialog->CBGetSettings(&fontSize, &printHeader);

		*changed = *changed ||
			fontSize    != itsFontSize ||
			printHeader != CBGetPTTextPrinter()->WillPrintHeader();

		itsFontSize = fontSize;
		CBGetPTTextPrinter()->ShouldPrintHeader(printHeader);
		}

	itsCBPrintSetupDialog = nullptr;
	return ok;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBPSPrinter::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		if (vers >= 1)
			{
			input >> itsFontSize;
			}

		JXPSPrinter::ReadXPSSetup(input);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBPSPrinter::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsFontSize;
	output << ' ';
	JXPSPrinter::WriteXPSSetup(output);
	output << ' ';
}

/******************************************************************************
 OpenDocument (virtual)

	Change the font to ensure monospace.

 ******************************************************************************/

bool
CBPSPrinter::OpenDocument()
{
	assert( itsTE != nullptr );

	JXGetApplication()->DisplayBusyCursor();	// changing font can take a while

	if (JXPSPrinter::OpenDocument())
		{
		assert( itsFontSize != kUnsetFontSize );
		itsTE->SetFontBeforePrintPS(itsFontSize);
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 CloseDocument (virtual)

	Resets the font.

 ******************************************************************************/

void
CBPSPrinter::CloseDocument()
{
	assert( itsTE != nullptr );

	JXPSPrinter::CloseDocument();
	itsTE->ResetFontAfterPrintPS();
	itsTE = nullptr;
}

/******************************************************************************
 CancelDocument (virtual)

	Resets the font.

 ******************************************************************************/

void
CBPSPrinter::CancelDocument()
{
	assert( itsTE != nullptr );

	JXPSPrinter::CancelDocument();
	itsTE->ResetFontAfterPrintPS();
	itsTE = nullptr;
}
