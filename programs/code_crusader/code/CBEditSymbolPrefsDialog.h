/******************************************************************************
 CBEditSymbolPrefsDialog.h

	Interface for the CBEditSymbolPrefsDialog class

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditSymbolPrefsDialog
#define _H_CBEditSymbolPrefsDialog

#include <JXDialogDirector.h>

class JXTextButton;
class JXTextCheckbox;

class CBEditSymbolPrefsDialog : public JXDialogDirector
{
public:

	CBEditSymbolPrefsDialog(const bool raiseTreeOnRightClick);

	virtual ~CBEditSymbolPrefsDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsRaiseTreeOnRightClickCB;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const bool raiseTreeOnRightClick);
	void	UpdateSettings();

	// not allowed

	CBEditSymbolPrefsDialog(const CBEditSymbolPrefsDialog& source);
	const CBEditSymbolPrefsDialog& operator=(const CBEditSymbolPrefsDialog& source);
};

#endif
