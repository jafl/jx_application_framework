/******************************************************************************
 JXCheckboxListDialog.h

	Interface for the JXCheckboxListDialog class

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCheckboxListDialog
#define _H_JXCheckboxListDialog

#include <JXDialogDirector.h>
#include <JPtrArray-JString.h>

class JXCheckbox;

class JXCheckboxListDialog : public JXDialogDirector
{
public:

	JXCheckboxListDialog(JXDirector* supervisor,
						 const JCharacter* windowTitle, const JCharacter* prompt,
						 const JPtrArray<JString>& choiceList,
						 const JPtrArray<JString>* shortcutList = NULL);

	virtual ~JXCheckboxListDialog();

	JBoolean	GetSelectedItems(JArray<JIndex>* indexList) const;
	void		SelectItem(const JIndex index);
	void		SelectAllItems();

private:

	JPtrArray<JXCheckbox>*	itsCBList;

private:

	void	BuildWindow(const JCharacter* windowTitle, const JCharacter* prompt,
						const JPtrArray<JString>& choiceList,
						const JPtrArray<JString>* shortcutList);

	// not allowed

	JXCheckboxListDialog(const JXCheckboxListDialog& source);
	const JXCheckboxListDialog& operator=(const JXCheckboxListDialog& source);
};

#endif
