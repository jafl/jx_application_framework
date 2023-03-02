/******************************************************************************
 JXDNDChooseDropActionDialog.cpp

	Asks the user which action to perform with the dropped data.

	BASE CLASS = JXRadioGroupDialog

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXDNDChooseDropActionDialog.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDNDChooseDropActionDialog::JXDNDChooseDropActionDialog
	(
	const JArray<Atom>&			actionList,
	const JPtrArray<JString>&	descriptionList,
	const Atom					defaultAction
	)
	:
	JXRadioGroupDialog(JGetString("WindowTitle::JXDNDChooseDropActionDialog"),
					   JGetString("Prompt::JXDNDChooseDropActionDialog"),
					   descriptionList),
	itsActionList(actionList)
{
	const JSize count = itsActionList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (itsActionList.GetElement(i) == defaultAction)
		{
			SelectItem(i);
			break;
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDNDChooseDropActionDialog::~JXDNDChooseDropActionDialog()
{
}

/******************************************************************************
 GetAction

 ******************************************************************************/

Atom
JXDNDChooseDropActionDialog::GetAction()
	const
{
	return itsActionList.GetElement(GetSelectedItem());
}
