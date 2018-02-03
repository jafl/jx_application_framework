/******************************************************************************
 WarPlayer.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_WarPlayer
#define _H_WarPlayer

#include "WWPlayer.h"

class WarSocket;

class WarPlayer : public WWPlayer
{
public:

	WarPlayer(const JIndex playerIndex, WarSocket* socket);

	virtual ~WarPlayer();

	WarSocket*	GetSocket() const;

	virtual void	WriteState(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
