/******************************************************************************
 CMArrayIndexInput.h

	Interface for the CMArrayIndexInput class

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMArrayIndexInput
#define _H_CMArrayIndexInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIntegerInput.h>

class CMArrayIndexInput : public JXIntegerInput
{
public:

	CMArrayIndexInput(JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CMArrayIndexInput();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

private:

	// not allowed

	CMArrayIndexInput(const CMArrayIndexInput& source);
	const CMArrayIndexInput& operator=(const CMArrayIndexInput& source);

public:

	// JBroadcaster messages

	static const JCharacter* kReturnKeyPressed;

	class ReturnKeyPressed : public JBroadcaster::Message
		{
		public:

			ReturnKeyPressed()
				:
				JBroadcaster::Message(kReturnKeyPressed)
				{ };
		};
};

#endif
