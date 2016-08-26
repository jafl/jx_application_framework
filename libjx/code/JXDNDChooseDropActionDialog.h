/******************************************************************************
 JXDNDChooseDropActionDialog.h

	Interface for the JXDNDChooseDropActionDialog class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDNDChooseDropActionDialog
#define _H_JXDNDChooseDropActionDialog

#include <JXRadioGroupDialog.h>
#include <X11/X.h>

class JXDNDChooseDropActionDialog : public JXRadioGroupDialog
{
public:

	JXDNDChooseDropActionDialog(const JArray<Atom>& actionList,
								const JPtrArray<JString>& descriptionList,
								const Atom defaultAction);

	virtual ~JXDNDChooseDropActionDialog();

	Atom	GetAction() const;

private:

	const JArray<Atom>&	itsActionList;

private:

	// not allowed

	JXDNDChooseDropActionDialog(const JXDNDChooseDropActionDialog& source);
	const JXDNDChooseDropActionDialog& operator=(const JXDNDChooseDropActionDialog& source);
};

#endif
