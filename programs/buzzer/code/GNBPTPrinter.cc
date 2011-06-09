/******************************************************************************
 GNBPTPrinter.cc

	BASE CLASS = JXPTPrinter, JPrefObject

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GNBPTPrinter.h"
#include "GNBPTPrintSetupDialog.h"

#include <jStreamUtil.h>
#include <jTime.h>

#include <jAssert.h>

const JCharacter* GNBPTPrinter::kPrintStarting = "PrintStarting::GNBPTPrinter";
const JCharacter* GNBPTPrinter::kPrintFinished = "PrintFinished::GNBPTPrinter";

const JFileVersion kCurrentPrefsVersion	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GNBPTPrinter::GNBPTPrinter
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	id
	)
	:
	JXPTPrinter(),
	JPrefObject(prefsMgr, id)
{
	itsPrintSelectionFlag  	= kJFalse;
	itsPrintNotesFlag  		= kJFalse;
	itsPrintClosedFlag	  	= kJFalse;
	itsPrintSetupDialog 	= NULL;

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GNBPTPrinter::~GNBPTPrinter()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXPTPrintSetupDialog*
GNBPTPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JCharacter*	printCmd,
	const JCharacter*	fileName,
	const JBoolean		printLineNumbers
	)
{
	assert( itsPrintSetupDialog == NULL );

	itsPrintSetupDialog = new GNBPTPrintSetupDialog();
	assert( itsPrintSetupDialog != NULL );
	itsPrintSetupDialog->BuildWindow(destination, printCmd, fileName,
									   printLineNumbers, itsPrintSelectionFlag,
									   itsPrintNotesFlag, itsPrintClosedFlag,
									   itsHasSelection);
	itsPrintSetupDialog->ShouldPrintSelection(itsPrintSelectionFlag);
	itsPrintSetupDialog->ShouldPrintNotes(itsPrintNotesFlag);
	itsPrintSetupDialog->ShouldPrintClosed(itsPrintClosedFlag);
	return itsPrintSetupDialog;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GNBPTPrinter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXPTPrinter::Receive(sender, message);
	if (sender == itsPrintSetupDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		Broadcast(PrintFinished());
		}
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns kJTrue if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
GNBPTPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message& message
	)
{
	assert( itsPrintSetupDialog != NULL );

	JBoolean changed;
	const JBoolean ok = JXPTPrinter::EndUserPrintSetup(message, &changed);
	if (ok)
		{
		itsPrintSelectionFlag	= itsPrintSetupDialog->WillPrintSelection();
		itsPrintNotesFlag 		= itsPrintSetupDialog->WillPrintNotes();
		itsPrintClosedFlag 		= itsPrintSetupDialog->WillPrintClosed();
		Broadcast(PrintStarting());
		}

	itsPrintSetupDialog = NULL;
	return ok;
}

/******************************************************************************
 Read/WritePrefs

 ******************************************************************************/

void
GNBPTPrinter::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version == 1)
		{
		input >> itsPrintSelectionFlag;
		input >> itsPrintNotesFlag;
		input >> itsPrintClosedFlag;
		ReadXPTSetup(input);
		}
}

void
GNBPTPrinter::WritePrefs
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion << ' ';
	output << itsPrintSelectionFlag << ' ';
	output << itsPrintNotesFlag << ' ';
	output << itsPrintClosedFlag << ' ';
	WriteXPTSetup(output);
}
