/******************************************************************************
 JXRadioGroupDialog.h

	Interface for the JXRadioGroupDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRadioGroupDialog
#define _H_JXRadioGroupDialog

#include "JXModalDialogDirector.h"
#include <jx-af/jcore/JPtrArray-JString.h>

class JXRadioGroup;

class JXRadioGroupDialog : public JXModalDialogDirector
{
public:

	JXRadioGroupDialog(const JString& windowTitle, const JString& prompt,
					   const JPtrArray<JString>& choiceList,
					   const JPtrArray<JString>* shortcutList = nullptr);

	~JXRadioGroupDialog() override;

	JIndex	GetSelectedItem() const;
	void	SelectItem(const JIndex index);

protected:

	JXRadioGroup*	GetRadioGroup() const;

private:

	JXRadioGroup*	itsRG;

private:

	void	BuildWindow(const JString& windowTitle, const JString& prompt,
						const JPtrArray<JString>& choiceList,
						const JPtrArray<JString>* shortcutList);
};


/******************************************************************************
 GetRadioGroup (protected)

 ******************************************************************************/

inline JXRadioGroup*
JXRadioGroupDialog::GetRadioGroup()
	const
{
	return itsRG;
}

#endif
