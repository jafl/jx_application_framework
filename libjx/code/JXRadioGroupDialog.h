/******************************************************************************
 JXRadioGroupDialog.h

	Interface for the JXRadioGroupDialog class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXRadioGroupDialog
#define _H_JXRadioGroupDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JPtrArray-JString.h>

class JXRadioGroup;

class JXRadioGroupDialog : public JXDialogDirector
{
public:

	JXRadioGroupDialog(JXDirector* supervisor,
					   const JCharacter* windowTitle, const JCharacter* prompt,
					   const JPtrArray<JString>& choiceList,
					   const JPtrArray<JString>* shortcutList = NULL);

	virtual ~JXRadioGroupDialog();

	JIndex	GetSelectedItem() const;
	void	SelectItem(const JIndex index);

protected:

	JXRadioGroup*	GetRadioGroup() const;

private:

	JXRadioGroup*	itsRG;

private:

	void	BuildWindow(const JCharacter* windowTitle, const JCharacter* prompt,
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
