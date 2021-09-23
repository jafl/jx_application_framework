/******************************************************************************
 JXCheckbox.h

	Interface for the JXCheckbox class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCheckbox
#define _H_JXCheckbox

#include "JXWidget.h"

class JXCheckbox : public JXWidget
{
public:

	virtual ~JXCheckbox();

	bool	IsChecked() const;
	void		SetState(const bool on);
	void		ToggleState();

	virtual void	SetShortcuts(const JString& list);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

protected:

	JXCheckbox(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	bool		DrawChecked() const;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

private:

	bool	itsIsCheckedFlag;
	bool	itsIsPushedFlag;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kPushed;

	class Pushed : public JBroadcaster::Message
		{
		public:

			Pushed(const bool on)
				:
				JBroadcaster::Message(kPushed),
				itsState(on)
				{ };

			bool
			IsChecked()
				const
			{
				return itsState;
			};

		private:

			bool itsState;
		};
};

/******************************************************************************
 IsChecked

	Returns true if the checkbox is in the "on" state.

 ******************************************************************************/

inline bool
JXCheckbox::IsChecked()
	const
{
	return itsIsCheckedFlag;
}

/******************************************************************************
 DrawChecked (protected)

	Returns true if the checkbox should be drawn as checked.

 ******************************************************************************/

inline bool
JXCheckbox::DrawChecked()
	const
{
	return ( itsIsCheckedFlag && !itsIsPushedFlag) ||
		(!itsIsCheckedFlag &&  itsIsPushedFlag);
}

/******************************************************************************
 ToggleState

 ******************************************************************************/

inline void
JXCheckbox::ToggleState()
{
	SetState(!itsIsCheckedFlag);
}

#endif
