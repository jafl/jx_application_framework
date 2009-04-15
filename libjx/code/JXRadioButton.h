/******************************************************************************
 JXRadioButton.h

	Interface for the JXRadioButton class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXRadioButton
#define _H_JXRadioButton

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>

class JXRadioGroup;

class JXRadioButton : public JXWidget
{
	friend class JXRadioGroup;

public:

	virtual ~JXRadioButton();

	JIndex		GetID() const;
	void		SetID(const JIndex id);

	JBoolean	IsChecked() const;
	void		Select();

	virtual void	SetShortcuts(const JCharacter* list);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

protected:

	JXRadioButton(const JIndex id, JXRadioGroup* enclosure,
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

	JIndex			itsID;
	JXRadioGroup*	itsRadioGroup;
	JBoolean		itsIsCheckedFlag;
	JBoolean		itsIsPushedFlag;

private:

	// called by JXRadioGroup

	void	Deselect();

	// not allowed

	JXRadioButton(const JXRadioButton& source);
	const JXRadioButton& operator=(const JXRadioButton& source);
};

/******************************************************************************
 GetID

 ******************************************************************************/

inline JIndex
JXRadioButton::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 SetID

 ******************************************************************************/

inline void
JXRadioButton::SetID
	(
	const JIndex id
	)
{
	itsID = id;
}

/******************************************************************************
 IsChecked

	Returns kJTrue if the radio button is in the "on" state.

 ******************************************************************************/

inline JBoolean
JXRadioButton::IsChecked()
	const
{
	return itsIsCheckedFlag;
}

/******************************************************************************
 DrawChecked (protected)

	Returns kJTrue if the radio button should be drawn in the "on" state.

 ******************************************************************************/

inline JBoolean
JXRadioButton::DrawChecked()
	const
{
	return JConvertToBoolean( itsIsCheckedFlag || itsIsPushedFlag );
}

/******************************************************************************
 Deselect (private)

 ******************************************************************************/

inline void
JXRadioButton::Deselect()
{
	itsIsCheckedFlag = kJFalse;
	Redraw();
}

#endif
