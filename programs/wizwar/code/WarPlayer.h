/******************************************************************************
 WarPlayer.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WarPlayer
#define _H_WarPlayer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "WWPlayer.h"

class WarSocket;

class WarPlayer : public WWPlayer
{
public:

	WarPlayer(const JIndex playerIndex, WarSocket* socket);

	virtual ~WarPlayer();

	WarSocket*	GetSocket() const;

	virtual void	WriteState(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	WarSocket*	itsSocket;

private:

	// not allowed

	WarPlayer(const WarPlayer& source);
	WarPlayer& operator=(const WarPlayer& source);
};


/******************************************************************************
 GetSocket

 ******************************************************************************/

inline WarSocket*
WarPlayer::GetSocket()
	const
{
	return itsSocket;
}

#endif
