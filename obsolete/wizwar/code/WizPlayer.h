/******************************************************************************
 WizPlayer.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WizPlayer
#define _H_WizPlayer

#include "WWPlayer.h"
#include <jColor.h>

class WizPlayer : public WWPlayer
{
public:

	WizPlayer(std::istream& input);

	virtual ~WizPlayer();

	JColorIndex	GetColor() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	// not allowed

	WizPlayer(const WizPlayer& source);
	WizPlayer& operator=(const WizPlayer& source);
};

#endif
