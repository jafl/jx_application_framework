/******************************************************************************
 JXTipOfTheDayDialog.h

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTipOfTheDayDialog
#define _H_JXTipOfTheDayDialog

#include "JXModalDialogDirector.h"

class JXStaticText;
class JXTextButton;
class JXTextCheckbox;

class JXTipOfTheDayDialog : public JXModalDialogDirector
{
public:

	JXTipOfTheDayDialog(const bool showStartupCB = false,
						const bool showAtStartup = false);

	~JXTipOfTheDayDialog() override;

	bool	ShowAtStartup() const;

private:

	JPtrArray<JString>*	itsTipList;
	JIndex				itsTipIndex;

	JXStaticText*	itsText;

// begin JXLayout

	JXTextCheckbox* itsShowAtStartupCB;
	JXTextButton*   itsNextTipButton;
	JXTextButton*   itsCloseButton;

// end JXLayout

private:

	void	BuildWindow(const bool showStartupCB,
						const bool showAtStartup);

	void	ParseTips();
	void	AddTip(JString tip);
	void	DisplayTip();
};

#endif
