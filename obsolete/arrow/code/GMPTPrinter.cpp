/******************************************************************************
 GMPTPrinter.cc

	BASE CLASS = JXPTPrinter, JPrefObject

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GMPTPrinter.h"
#include "GMPTPrintSetupDialog.h"

#include <jStreamUtil.h>
#include <jTime.h>

#include <jAssert.h>

const JCharacter* GMPTPrinter::kPrintStarting = "PrintStarting::GMPTPrinter";
const JCharacter* GMPTPrinter::kPrintFinished = "PrintFinished::GMPTPrinter";

const JFileVersion kCurrentPrefsVersion	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GMPTPrinter::GMPTPrinter
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	id
	)
	:
	JXPTPrinter(),
	JPrefObject(prefsMgr, id)
{
	itsPrintHeaderFlag	= kJFalse;
	itsPrintSetupDialog	= NULL;

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMPTPrinter::~GMPTPrinter()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 CreatePrintSetupDialog (virtual protected)

	Derived class can override to create an extended dialog.

 ******************************************************************************/

JXPTPrintSetupDialog*
GMPTPrinter::CreatePrintSetupDialog
	(
	const Destination	destination,
	const JCharacter*	printCmd,
	const JCharacter*	fileName,
	const JBoolean		printLineNumbers
	)
{
	assert( itsPrintSetupDialog == NULL );

	itsPrintSetupDialog = new GMPTPrintSetupDialog();
	assert( itsPrintSetupDialog != NULL );
	itsPrintSetupDialog->BuildWindow(destination, printCmd, fileName,
									   printLineNumbers, itsPrintHeaderFlag);
	itsPrintSetupDialog->ShouldPrintHeader(itsPrintHeaderFlag);
	return itsPrintSetupDialog;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMPTPrinter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXPTPrinter::Receive(sender, message);
	if (sender == itsPrintSetupDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		Broadcast(PrintFinished());
		itsPrintSetupDialog	= NULL;
		}
}

/******************************************************************************
 EndUserPrintSetup (virtual protected)

	Returns kJTrue if caller should continue the printing process.
	Derived classes can override this to extract extra information.

 ******************************************************************************/

JBoolean
GMPTPrinter::EndUserPrintSetup
	(
	const JBroadcaster::Message& message
	)
{
	assert( itsPrintSetupDialog != NULL );

	JBoolean changed;
	const JBoolean ok = JXPTPrinter::EndUserPrintSetup(message, &changed);
	if (ok)
		{
		itsPrintHeaderFlag = itsPrintSetupDialog->WillPrintHeader();
		Broadcast(PrintStarting());
		}

	itsPrintSetupDialog = NULL;
	return ok;
}

/******************************************************************************
 Read/WritePrefs

 ******************************************************************************/

void
GMPTPrinter::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version == 1)
		{
		input >> itsPrintHeaderFlag;
		ReadXPTSetup(input);
		}
}

void
GMPTPrinter::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ';
	output << itsPrintHeaderFlag << ' ';
	WriteXPTSetup(output);
}
