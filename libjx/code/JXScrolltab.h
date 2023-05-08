/******************************************************************************
 JXScrolltab.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXScrolltab
#define _H_JXScrolltab

#include "JXWidget.h"

class JXScrollbar;
class JXTextMenu;

class JXScrolltab : public JXWidget
{
public:

	JXScrolltab(JXScrollbar* enclosure, const JCoordinate value);

	~JXScrolltab() override;

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

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	HandleMouseEnter() override;
	void	HandleMouseLeave() override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	JXScrollbar*	itsScrollbar;	// owns us
	JCoordinate		itsValue;
	JXTextMenu*		itsActionMenu;	// can be nullptr

private:

	void	OpenActionMenu(const JPoint& pt, const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers);
	void	HandleActionMenu(const JIndex index);
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
