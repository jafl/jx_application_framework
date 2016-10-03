/******************************************************************************
 ASKDialog.cc

	Asks for the passphrase and prints it to stdout.

	BASE CLASS = public JXGetStringDialog

	Copyright (C) 2006 by New Planet Software, Inc.. All rights reserved.

 *****************************************************************************/

#include "ASKDialog.h"
#include "ASKApp.h"
#include "askGlobals.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXPasswordInput.h>
#include <JXTextButton.h>
#include <jAssert.h>

// string ID's

/******************************************************************************
 Constructor

 *****************************************************************************/

ASKDialog::ASKDialog
	(
	JXDirector* supervisor
	)
	:
	JXGetStringDialog(supervisor, JGetString("Title::ASKDialog"),
					  JGetString("Prompt::ASKDialog"), NULL, kJTrue, kJTrue)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ASKDialog::~ASKDialog()
{
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
ASKDialog::OKToDeactivate()
{
	if (JXGetStringDialog::OKToDeactivate())
		{
		cout << GetString() << endl;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
