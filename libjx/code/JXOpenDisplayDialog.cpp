/******************************************************************************
 JXOpenDisplayDialog.cpp

	BASE CLASS = JXGetStringDialog

	Copyright (C) 1996-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXOpenDisplayDialog.h>
#include <jXGlobals.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kWindowTitleID = "WindowTitle::JXOpenDisplayDialog";
static const JCharacter* kPromptID      = "Prompt::JXOpenDisplayDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXOpenDisplayDialog::JXOpenDisplayDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXGetStringDialog(supervisor, JGetString(kWindowTitleID),
					  JGetString(kPromptID), "", kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXOpenDisplayDialog::~JXOpenDisplayDialog()
{
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXOpenDisplayDialog::OKToDeactivate()
{
	if (!JXGetStringDialog::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	const JString& displayName = GetString();
	JIndex displayIndex;
	if ((JXGetApplication())->OpenDisplay(displayName, &displayIndex))
		{
		itsDisplayIndex = displayIndex;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
