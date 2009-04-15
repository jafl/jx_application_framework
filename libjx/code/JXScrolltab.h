/******************************************************************************
 JXScrolltab.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXScrolltab
#define _H_JXScrolltab

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>

class JXScrollbar;
class JXTextMenu;

class JXScrolltab : public JXWidget
{
public:

	JXScrolltab(JXScrollbar* enclosure, const JCoordinate value);

	virtual ~JXScrolltab();

	void	ScrollToTab() const;

	JCoordinate	GetValue() const;
	void		SetValue(const JCoordinate value);
	void		AdjustValue(const JCoordinate delta);
	void		ScaleValue(const JFloat scaleFactor);

	// called by JXScrollbar

	void	PlaceHoriz(const JCoordinate xmin, const JCoordinate xmax,
					   const JFloat scale);
	void	PlaceVert(const JCoordinate ymin, const JCoordinate ymax,
					  const JFloat scale);

protected:

	void	UpdatePosition();

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual void	HandleMouseEnter();
	virtual void	HandleMouseLeave();

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXScrollbar*	itsScrollbar;	// owns us
	JCoordinate		itsValue;
	JXTextMenu*		itsActionMenu;	// can be NULL

private:

	void	OpenActionMenu(const JPoint& pt, const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers);
	void	HandleActionMenu(const JIndex index);

	// not allowed

	JXScrolltab(const JXScrolltab& source);
	const JXScrolltab& operator=(const JXScrolltab& source);
};

/******************************************************************************
 GetValue

 ******************************************************************************/

inline JCoordinate
JXScrolltab::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 SetValue

 ******************************************************************************/

inline void
JXScrolltab::SetValue
	(
	const JCoordinate value
	)
{
	if (value != itsValue)
		{
		itsValue = value;
		UpdatePosition();
		}
}

/******************************************************************************
 AdjustValue

 ******************************************************************************/

inline void
JXScrolltab::AdjustValue
	(
	const JCoordinate delta
	)
{
	SetValue(itsValue + delta);
}

#endif
