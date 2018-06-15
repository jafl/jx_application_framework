/******************************************************************************
 JXRadioGroupDialog.h

	Interface for the JXRadioGroupDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRadioGroupDialog
#define _H_JXRadioGroupDialog

#include "JXDialogDirector.h"
#include <JPtrArray-JString.h>

class JXRadioGroup;

class JXRadioGroupDialog : public JXDialogDirector
{
public:

	JXRadioGroupDialog(JXDirector* supervisor,
					   const JString& windowTitle, const JString& prompt,
					   const JPtrArray<JString>& choiceList,
					   const JPtrArray<JString>* shortcutList = nullptr);

	virtual ~JXRadioGroupDialog();

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

	// not allowed

	JXRadioGroupDialog(const JXRadioGroupDialog& source);
	const JXRadioGroupDialog& operator=(const JXRadioGroupDialog& source);
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
