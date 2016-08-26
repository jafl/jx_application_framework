/******************************************************************************
 JVersionSocket.h

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVersionSocket
#define _H_JVersionSocket

#include <JPrefObject.h>
#include <JString.h>
#include <j_prep_ace.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

template <ACE_PEER_STREAM_1>
class JVersionSocket : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>,
					   public JPrefObject
{
public:

	JVersionSocket(const JCharacter* host, const JCharacter* path,
				   JPrefsManager* prefsMgr, const JPrefID& prefID);

	JVersionSocket();	// DO NOT CALL -- required to instantiate ACE_Connector

	virtual ~JVersionSocket();

	const JString&	GetLatestVersion() const;
	JBoolean		TimeToCheck() const;
	JBoolean		TimeToRemind();

	virtual int	open(void*);
	virtual int	handle_input(ACE_HANDLE);

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	const JString	itsHostName;
	const JString	itsPath;
	JString			itsRecvBuffer;

	JPrefsManager*	itsPrefsMgr;	// not owned
	JPrefID			itsPrefID;
	JString			itsVersion;
	time_t			itsNextServerTime;
	JArray<time_t>*	itsReminderList;

private:

	void	CheckVersion(const JCharacter* vers);
	void	ReadState(JString* vers, JArray<time_t>* reminders);
};


/******************************************************************************
 GetLatestVersion

 ******************************************************************************/

template <ACE_PEER_STREAM_1>
inline const JString&
JVersionSocket<ACE_PEER_STREAM_2>::GetLatestVersion()
	const
{
	return itsVersion;
}

#include <JVersionSocket.tmpl>

#endif
