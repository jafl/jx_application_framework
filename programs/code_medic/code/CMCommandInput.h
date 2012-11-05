/******************************************************************************
 CMCommandInput.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMCommandInput
#define _H_CMCommandInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTEBase.h>

class CMCommandInput : public JXTEBase
{
public:

	CMCommandInput(JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual	~CMCommandInput();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleFocusEvent();
	virtual void	HandleUnfocusEvent();

private:

	// not allowed

	CMCommandInput(const CMCommandInput& source);
	const CMCommandInput& operator=(const CMCommandInput& source);

public:

	// JBroadcaster messages

	static const JCharacter* kReturnKeyPressed;
	static const JCharacter* kTabKeyPressed;
	static const JCharacter* kUpArrowKeyPressed;
	static const JCharacter* kDownArrowKeyPressed;

	class ReturnKeyPressed : public JBroadcaster::Message
		{
		public:

			ReturnKeyPressed()
				:
				JBroadcaster::Message(kReturnKeyPressed)
				{ };
		};

	class TabKeyPressed : public JBroadcaster::Message
		{
		public:

			TabKeyPressed()
				:
				JBroadcaster::Message(kTabKeyPressed)
				{ };
		};

	class UpArrowKeyPressed : public JBroadcaster::Message
		{
		public:

			UpArrowKeyPressed()
				:
				JBroadcaster::Message(kUpArrowKeyPressed)
				{ };
		};

	class DownArrowKeyPressed : public JBroadcaster::Message
		{
		public:

			DownArrowKeyPressed()
				:
				JBroadcaster::Message(kDownArrowKeyPressed)
				{ };
		};
};

#endif
