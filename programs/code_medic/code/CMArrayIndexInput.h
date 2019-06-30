/******************************************************************************
 CMArrayIndexInput.h

	Interface for the CMArrayIndexInput class

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CMArrayIndexInput
#define _H_CMArrayIndexInput

#include <JXIntegerInput.h>

class CMArrayIndexInput : public JXIntegerInput
{
public:

	CMArrayIndexInput(JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CMArrayIndexInput();

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

private:

	// not allowed

	CMArrayIndexInput(const CMArrayIndexInput& source);
	const CMArrayIndexInput& operator=(const CMArrayIndexInput& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kReturnKeyPressed;

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
