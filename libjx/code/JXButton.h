/******************************************************************************
 JXButton.h

	Interface for the JXButton class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXButton
#define _H_JXButton

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>

class JXButton : public JXWidget
{
public:

	virtual ~JXButton();

	void	Push();

	virtual void	SetShortcuts(const JCharacter* list);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

protected:

	JXButton(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	JBoolean	IsPushed() const;

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

private:

	JBoolean	itsIsReturnButtonFlag;
	JBoolean	itsIsPushedFlag;

private:

	// not allowed

	JXButton(const JXButton& source);
	const JXButton& operator=(const JXButton& source);

public:

	// JBroadcaster messages

	static const JCharacter* kPushed;

	class Pushed : public JBroadcaster::Message
		{
		public:

			Pushed()
				:
				JBroadcaster::Message(kPushed)
				{ };
		};
};


/******************************************************************************
 IsPushed (protected)

 ******************************************************************************/

inline JBoolean
JXButton::IsPushed()
	const
{
	return itsIsPushedFlag;
}

#endif
