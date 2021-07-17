/******************************************************************************
 JXOpenDisplayDialog.cpp

	BASE CLASS = JXGetStringDialog

	Copyright (C) 1996-2008 by John Lindal.

 ******************************************************************************/

#include "JXOpenDisplayDialog.h"
#include "jXGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXOpenDisplayDialog::JXOpenDisplayDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXGetStringDialog(supervisor, JGetString("WindowTitle::JXOpenDisplayDialog"),
					  JGetString("Prompt::JXOpenDisplayDialog"), JString::empty, true)
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

bool
JXOpenDisplayDialog::OKToDeactivate()
{
	if (!JXGetStringDialog::OKToDeactivate())
		{
		return false;
		}
	else if (Cancelled())
		{
		return true;
		}

	const JString& displayName = GetString();
	JIndex displayIndex;
	if (JXGetApplication()->OpenDisplay(displayName, &displayIndex))
		{
		itsDisplayIndex = displayIndex;
		return true;
		}
	else
		{
		return false;
		}
}
