/******************************************************************************
 JXCheckboxListDialog.h

	Interface for the JXCheckboxListDialog class

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCheckboxListDialog
#define _H_JXCheckboxListDialog

#include "jx-af/jx/JXDialogDirector.h"
#include <jx-af/jcore/JPtrArray-JString.h>

class JXCheckbox;

class JXCheckboxListDialog : public JXDialogDirector
{
public:

	JXCheckboxListDialog(JXDirector* supervisor,
						 const JString& windowTitle, const JString& prompt,
						 const JPtrArray<JString>& choiceList,
						 const JPtrArray<JString>* shortcutList = nullptr);

	virtual ~JXCheckboxListDialog();

	bool	GetSelectedItems(JArray<JIndex>* indexList) const;
	void		SelectItem(const JIndex index);
	void		SelectAllItems();

private:

	JPtrArray<JXCheckbox>*	itsCBList;

private:

	void	BuildWindow(const JString& windowTitle, const JString& prompt,
						const JPtrArray<JString>& choiceList,
						const JPtrArray<JString>* shortcutList);
};

#endif
