/******************************************************************************
 CBPTPrinter.cpp

	BASE CLASS = JXPTPrinter, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBPTPrinter.h"
#include "CBPTPrintSetupDialog.h"
#include "cbGlobals.h"
#include <jStreamUtil.h>
#include <jTime.h>
#include <jAssert.h>

const JSize kPrintHeaderLineCount = 4;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 0 was written by JXTEBase::WritePrintPlainTextSetup().

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPTPrinter::CBPTPrinter()
	:
	JXPTPrinter(),
	JPrefObject(CBGetPrefsManager(), kCBPrintPlainTextID)
{
	itsPrintHeaderFlag    = true;
	itsCBPrintSetupDialog = nullptr;

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPTPrinter::~CBPTPrinter()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXPTPrintSetupDialog*
CBPTPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JString&		printCmd,
	const JString&		fileName,
	const bool		printLineNumbers
	)
{
	assert( itsCBPrintSetupDialog == nullptr );

	itsCBPrintSetupDialog =
		CBPTPrintSetupDialog::Create(destination, printCmd, fileName,
									 printLineNumbers, itsPrintHeaderFlag);
	return itsCBPrintSetupDialog;
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns true if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

bool
CBPTPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message&	message,
	bool*						changed
	)
{
	assert( itsCBPrintSetupDialog != nullptr );

	const bool ok = JXPTPrinter::EndUserPrintSetup(message, changed);
	if (ok)
		{
		*changed = *changed ||
			itsCBPrintSetupDialog->ShouldPrintHeader() != itsPrintHeaderFlag;

		itsPrintHeaderFlag = itsCBPrintSetupDialog->ShouldPrintHeader();
		if (GetPageHeight() <= GetHeaderLineCount())
			{
			SetPageHeight(GetHeaderLineCount() + 1);
			*changed = true;
			}
		}

	itsCBPrintSetupDialog = nullptr;
	return ok;
}

/******************************************************************************
 Print header and footer (virtual protected)

	Derived classes can override these functions if they want to
	print a header or a footer.  The default is to do nothing.

 ******************************************************************************/

JSize
CBPTPrinter::GetHeaderLineCount()
	const
{
	return (itsPrintHeaderFlag ? kPrintHeaderLineCount : 0);
}

void
CBPTPrinter::PrintHeader
	(
	std::ostream&		output,
	const JIndex	pageIndex
	)
{
	if (itsPrintHeaderFlag)
		{
		const JString dateStr = JGetTimeStamp();

		JString pageStr(pageIndex, 0);
		pageStr.Prepend("Page ");

		itsHeaderName.Print(output);
		output << '\n';

		dateStr.Print(output);
		const JInteger spaceCount =
			GetPageWidth() - dateStr.GetCharacterCount() - pageStr.GetCharacterCount();
		for (JInteger i=1; i<=spaceCount; i++)
			{
			output << ' ';
			}
		pageStr.Print(output);
		output << "\n\n\n";
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBPTPrinter::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers == 0)
		{
		JString printCmd;
		JSize pageHeight;
		input >> printCmd >> pageHeight;
		JIgnoreUntil(input, '\1');

		SetPrintCmd(printCmd);
		SetPageHeight(pageHeight);
		}
	else if (vers <= kCurrentSetupVersion)
		{
		input >> JBoolFromString(itsPrintHeaderFlag);
		JXPTPrinter::ReadXPTSetup(input);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBPTPrinter::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << JBoolToString(itsPrintHeaderFlag);

	output << ' ';
	JXPTPrinter::WriteXPTSetup(output);

	output << ' ';
}
