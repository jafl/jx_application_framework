/******************************************************************************
 CBPSPrinter.cpp

	BASE CLASS = JXPSPrinter, JPrefObject

	Copyright (C) 1999 by John Lindal. All rights reserved.

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
	itsTE                 = NULL;
	itsCBPrintSetupDialog = NULL;

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
	const JCharacter*	printCmd,
	const JCharacter*	fileName,
	const JBoolean		collate,
	const JBoolean		bw
	)
{
	assert( itsCBPrintSetupDialog == NULL );

	if (itsFontSize == kUnsetFontSize)
		{
		JString fontName;
		CBGetPrefsManager()->GetDefaultFont(&fontName, &itsFontSize);

		JArray<JIndexRange> matchList;
		if (nxmRegex.Match(fontName, &matchList))
			{
			const JString hStr = fontName.GetSubstring(matchList.GetElement(2));
			const JBoolean ok  = hStr.ConvertToUInt(&itsFontSize);
			assert( ok );
			itsFontSize--;
			}
		}

	itsCBPrintSetupDialog =
		CBPSPrintSetupDialog::Create(destination, printCmd, fileName,
									 collate, bw, itsFontSize,
									 (CBGetPTTextPrinter())->WillPrintHeader());
	return itsCBPrintSetupDialog;
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns kJTrue if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
CBPSPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message&	message,
	JBoolean*						changed
	)
{
	assert( itsCBPrintSetupDialog != NULL );

	const JBoolean ok = JXPSPrinter::EndUserPrintSetup(message, changed);
	if (ok)
		{
		JSize fontSize;
		JBoolean printHeader;
		itsCBPrintSetupDialog->CBGetSettings(&fontSize, &printHeader);

		*changed = JI2B(*changed ||
			fontSize    != itsFontSize ||
			printHeader != (CBGetPTTextPrinter())->WillPrintHeader());

		itsFontSize = fontSize;
		(CBGetPTTextPrinter())->ShouldPrintHeader(printHeader);
		}

	itsCBPrintSetupDialog = NULL;
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

	Change the font to insure monospace.

 ******************************************************************************/

JBoolean
CBPSPrinter::OpenDocument()
{
	assert( itsTE != NULL );

	(JXGetApplication())->DisplayBusyCursor();	// changing font can take a while

	if (JXPSPrinter::OpenDocument())
		{
		assert( itsFontSize != kUnsetFontSize );
		itsTE->SetFontBeforePrintPS(itsFontSize);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CloseDocument (virtual)

	Resets the font.

 ******************************************************************************/

void
CBPSPrinter::CloseDocument()
{
	assert( itsTE != NULL );

	JXPSPrinter::CloseDocument();
	itsTE->ResetFontAfterPrintPS();
	itsTE = NULL;
}

/******************************************************************************
 CancelDocument (virtual)

	Resets the font.

 ******************************************************************************/

void
CBPSPrinter::CancelDocument()
{
	assert( itsTE != NULL );

	JXPSPrinter::CancelDocument();
	itsTE->ResetFontAfterPrintPS();
	itsTE = NULL;
}
