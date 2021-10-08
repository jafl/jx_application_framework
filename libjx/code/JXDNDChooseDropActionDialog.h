/******************************************************************************
 JXDNDChooseDropActionDialog.h

	Interface for the JXDNDChooseDropActionDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDNDChooseDropActionDialog
#define _H_JXDNDChooseDropActionDialog

#include "jx-af/jx/JXRadioGroupDialog.h"
#include <X11/X.h>

class JXDNDChooseDropActionDialog : public JXRadioGroupDialog
{
public:

	JXDNDChooseDropActionDialog(const JArray<Atom>& actionList,
								const JPtrArray<JString>& descriptionList,
								const Atom defaultAction);

	~JXDNDChooseDropActionDialog();

	Atom	GetAction() const;

private:

	const JArray<Atom>&	itsActionList;
};

#endif
