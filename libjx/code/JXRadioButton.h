/******************************************************************************
 JXRadioButton.h

	Interface for the JXRadioButton class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRadioButton
#define _H_JXRadioButton

#include "JXWidget.h"

class JXRadioGroup;

class JXRadioButton : public JXWidget
{
	friend class JXRadioGroup;

public:

	~JXRadioButton() override;

	JIndex	GetID() const;
	void	SetID(const JIndex id);

	bool	IsChecked() const;
	void	Select();

	virtual void	SetShortcuts(const JString& list);
	void			HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

protected:

	JXRadioButton(const JIndex id, JXRadioGroup* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	bool	DrawChecked() const;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

private:

	JIndex			itsID;
	JXRadioGroup*	itsRadioGroup;
	bool		itsIsCheckedFlag;
	bool		itsIsPushedFlag;

private:

	// called by JXRadioGroup

	void	Deselect();
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

	Returns true if the radio button is in the "on" state.

 ******************************************************************************/

inline bool
JXRadioButton::IsChecked()
	const
{
	return itsIsCheckedFlag;
}

/******************************************************************************
 DrawChecked (protected)

	Returns true if the radio button should be drawn in the "on" state.

 ******************************************************************************/

inline bool
JXRadioButton::DrawChecked()
	const
{
	return itsIsCheckedFlag || itsIsPushedFlag;
}

/******************************************************************************
 Deselect (private)

 ******************************************************************************/

inline void
JXRadioButton::Deselect()
{
	itsIsCheckedFlag = false;
	Redraw();
}

#endif
