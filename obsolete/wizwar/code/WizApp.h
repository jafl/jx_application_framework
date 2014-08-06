/******************************************************************************
 WizApp.h

	Copyright � 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WizApp
#define _H_WizApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>
#include "Wiz2War.h"
#include <JString.h>

class WizChatDirector;
class WizConnectDialog;

class WizApp : public JXApplication
{
public:

	WizApp(int* argc, char* argv[]);

	virtual ~WizApp();

	const JCharacter*	GetPlayerName() const;

	static void	InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JString	itsServerAddr;
	JString	itsPlayerName;

	WizChatDirector*	itsChatDirector;

	// NULL unless in use

	WizConnectDialog*	itsConnectDialog;

private:

	void		Connect(const JSize argc, char* argv[]);
	void		RequestConnectInfo();
	void		ProcessConnectInfo();
	JBoolean	NotifyConnectionRefused(const Wiz2War::ConnectionRefused& info);

	void	ReadProgramPrefs();
	void	WriteProgramPrefs();

	// not allowed

	WizApp(const WizApp& source);
	const WizApp& operator=(const WizApp& source);
};

/******************************************************************************
 GetPlayerName

 ******************************************************************************/

inline const JCharacter*
WizApp::GetPlayerName()
	const
{
	return itsPlayerName;
}

#endif
