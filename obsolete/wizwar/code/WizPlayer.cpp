/******************************************************************************
 WizPlayer.cpp

	Implements client specific functions.

	BASE CLASS = WWPlayer

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "WizPlayer.h"
#include <JColormap.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	This is the dual of WriteState().

 ******************************************************************************/

WizPlayer::WizPlayer
	(
	istream& input
	)
	:
	WWPlayer(0)
{
	input >> itsPlayerIndex >> itsName >> itsEmail;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WizPlayer::~WizPlayer()
{
}

/******************************************************************************
 GetColor

 ******************************************************************************/

JColorIndex
WizPlayer::GetColor()
	const
{
		 if (itsPlayerIndex == 1)	return (JGetCurrColormap())->GetBlueColor();
	else if (itsPlayerIndex == 2)	return (JGetCurrColormap())->GetRedColor();
	else if (itsPlayerIndex == 3)	return (JGetCurrColormap())->GetGreenColor();
	else if (itsPlayerIndex == 4)	return (JGetCurrColormap())->GetMagentaColor();
	else if (itsPlayerIndex == 5)	return (JGetCurrColormap())->GetCyanColor();
	else if (itsPlayerIndex == 6)	return (JGetCurrColormap())->GetOrangeColor();
	else if (itsPlayerIndex == 7)	return (JGetCurrColormap())->GetYellowColor();
	else if (itsPlayerIndex == 8)	return (JGetCurrColormap())->GetBrownColor();
	else
		{
		assert( 0 /* WizPlayer::GetColor() needs more colors */ );
		return 0;
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
WizPlayer::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	WWPlayer::Receive(sender, message);
}
