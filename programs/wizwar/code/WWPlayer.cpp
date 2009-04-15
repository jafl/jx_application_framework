/******************************************************************************
 WWPlayer.cpp

	Maintains all the information about a player.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
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
	istream& input
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
	ostream& output
	)
	const
{
	assert( 0 /* Forgot to override WWPlayer::WriteState() */ );
}
