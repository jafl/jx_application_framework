/******************************************************************************
 ChooseWidgetDialog.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ChooseWidgetDialog
#define _H_ChooseWidgetDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextMenu;

class ChooseWidgetDialog : public JXModalDialogDirector
{
public:

	ChooseWidgetDialog();

	~ChooseWidgetDialog() override;

	JIndex	GetWidgetIndex() const;

private:

	JIndex	itsSelectedIndex;

// begin JXLayout

	JXTextMenu* itsWidgetMenu;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
