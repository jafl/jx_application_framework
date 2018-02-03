/******************************************************************************
 WWPlayer.cpp

	Maintains all the information about a player.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "WWPlayer.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WWPlayer::WWPlayer
	(
	const JIndex playerIndex
	)
{
	itsPlayerIndex = playerIndex;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WWPlayer::~WWPlayer()
{
}

/******************************************************************************
 ReadIdentity

 ******************************************************************************/

void
WWPlayer::ReadIdentity
	(
	std::istream& input
	)
{
	input >> itsName >> itsEmail;
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
WWPlayer::WriteState
	(
	std::ostream& output
	)
	const
{
	assert( 0 /* Forgot to override WWPlayer::WriteState() */ );
}
