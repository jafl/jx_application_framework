/******************************************************************************
 JXCheckbox.h

	Interface for the JXCheckbox class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCheckbox
#define _H_JXCheckbox

#include <JXWidget.h>

class JXCheckbox : public JXWidget
{
public:

	virtual ~JXCheckbox();

	JBoolean	IsChecked() const;
	void		SetState(const JBoolean on);
	void		ToggleState();

	virtual void	SetShortcuts(const JCharacter* list);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

protected:

	JXCheckbox(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	JBoolean		DrawChecked() const;

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

	JBoolean	itsIsCheckedFlag;
	JBoolean	itsIsPushedFlag;

private:

	// not allowed

	JXCheckbox(const JXCheckbox& source);
	const JXCheckbox& operator=(const JXCheckbox& source);

public:

	// JBroadcaster messages

	static const JCharacter* kPushed;

	class Pushed : public JBroadcaster::Message
		{
		public:

			Pushed(const JBoolean on)
				:
				JBroadcaster::Message(kPushed),
				itsState(on)
				{ };

			JBoolean
			IsChecked()
				const
			{
				return itsState;
			};

		private:

			JBoolean itsState;
		};
};

/******************************************************************************
 IsChecked

	Returns kJTrue if the checkbox is in the "on" state.

 ******************************************************************************/

inline JBoolean
JXCheckbox::IsChecked()
	const
{
	return itsIsCheckedFlag;
}

/******************************************************************************
 DrawChecked (protected)

	Returns kJTrue if the checkbox should be drawn as checked.

 ******************************************************************************/

inline JBoolean
JXCheckbox::DrawChecked()
	const
{
	return JConvertToBoolean(
		( itsIsCheckedFlag && !itsIsPushedFlag) ||
		(!itsIsCheckedFlag &&  itsIsPushedFlag));
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
