/******************************************************************************
 JXDNDChooseDropActionDialog.cpp

	Asks the user which action to perform with the dropped data.

	BASE CLASS = JXRadioGroupDialog

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDNDChooseDropActionDialog.h>
#include <jXGlobals.h>
#include <jAssert.h>

static const JCharacter* kWindowTitle = "Choose Drop Action";
static const JCharacter* kPromptText  = "Choose the action to perform:";

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
	JXRadioGroupDialog(JXGetApplication(),
					   kWindowTitle, kPromptText, descriptionList),
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
