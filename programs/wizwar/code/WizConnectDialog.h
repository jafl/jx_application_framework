/******************************************************************************
 WizConnectDialog.h

	Interface for the WizConnectDialog class

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WizConnectDialog
#define _H_WizConnectDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JString;
class JXTextButton;
class JXInputField;

class WizConnectDialog : public JXDialogDirector
{
public:

	WizConnectDialog(JXDirector* supervisor,
					 const JCharacter* serverAddr, const JCharacter* playerName);

	virtual ~WizConnectDialog();

	virtual void	Activate();

	void	GetConnectInfo(JString* serverAddr, JString* playerName) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

    JXTextButton* itsHelpButton;
    JXInputField* itsServerAddr;
    JXInputField* itsPlayerName;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* serverAddr, const JCharacter* playerName);

	// not allowed

	WizConnectDialog(const WizConnectDialog& source);
	const WizConnectDialog& operator=(const WizConnectDialog& source);
};

#endif
