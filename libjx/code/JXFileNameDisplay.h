/******************************************************************************
 JXFileNameDisplay.h

	Interface for the JXFileNameDisplay class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFileNameDisplay
#define _H_JXFileNameDisplay

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStaticText.h>

class JXFileNameDisplay : public JXStaticText
{
public:

	JXFileNameDisplay(const JCharacter* text, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~JXFileNameDisplay();

protected:

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	// not allowed

	JXFileNameDisplay(const JXFileNameDisplay& source);
	const JXFileNameDisplay& operator=(const JXFileNameDisplay& source);
};

#endif
