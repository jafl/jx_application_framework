/******************************************************************************
 CMArrayExprInput.h

	Interface for the CMArrayExprInput class

	Copyright � 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMArrayExprInput
#define _H_CMArrayExprInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class CMArrayExprInput : public JXInputField
{
public:

	CMArrayExprInput(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~CMArrayExprInput();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

private:

	// not allowed

	CMArrayExprInput(const CMArrayExprInput& source);
	const CMArrayExprInput& operator=(const CMArrayExprInput& source);

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
