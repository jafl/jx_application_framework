/******************************************************************************
 JVersionSocket.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVersionSocket
#define _H_JVersionSocket

#include "jx-af/jcore/JPrefObject.h"
#include "jx-af/jcore/JString.h"
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

template <ACE_PEER_STREAM_1>
class JVersionSocket : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>,
					   public JPrefObject
{
public:

	JVersionSocket(const JString& host, const JString& path,
				   JPrefsManager* prefsMgr, const JPrefID& prefID);

	JVersionSocket();	// DO NOT CALL -- required to instantiate ACE_Connector

	~JVersionSocket() override;

	const JString&	GetLatestVersion() const;
	bool			TimeToCheck() const;
	bool			TimeToRemind();

	int	open(void*) override;
	int	handle_input(ACE_HANDLE) override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

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

	void	CheckVersion(const JString& vers);
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

#include "JVersionSocket.tmpl"

#endif
