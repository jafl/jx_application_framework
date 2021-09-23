/******************************************************************************
 JXButton.h

	Interface for the JXButton class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXButton
#define _H_JXButton

#include "JXWidget.h"

class JXButton : public JXWidget
{
public:

	virtual ~JXButton();

	void	Push();

	virtual void	SetShortcuts(const JString& list);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

protected:

	JXButton(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	bool	IsPushed() const;

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JRect	GetFrameForFTC() const override;

private:

	bool	itsIsReturnButtonFlag;
	bool	itsIsPushedFlag;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kPushed;

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

inline bool
JXButton::IsPushed()
	const
{
	return itsIsPushedFlag;
}

#endif
