/******************************************************************************
 JXScrollbar.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXScrollbar
#define _H_JXScrollbar

#include "JXWidget.h"
#include <jMath.h>

class JPainter;
class JXScrolltab;
class JXTextMenu;

class JXScrollbar : public JXWidget
{
public:

	enum
	{
		kMinValue = 0
	};

public:

	JXScrollbar(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXScrollbar();

	JCoordinate	GetValue() const;
	void		SetValue(const JCoordinate value);
	void		AdjustValue(const JCoordinate delta);
	void		StepLine(const JCoordinate lineCount);
	void		StepPage(const JFloat pageCount);

	JCoordinate	GetStepSize() const;
	void		SetStepSize(const JCoordinate step);

	JCoordinate	GetPageStepSize() const;
	void		SetPageStepSize(const JCoordinate step);

	JCoordinate	GetMaxValue() const;
	void		SetMaxValue(const JCoordinate maxValue);

	bool	IsAtMin();
	void		ScrollToMin();
	bool	IsAtMax();
	void		ScrollToMax();

	JFloat	GetScrollDelay() const;
	void	SetScrollDelay(const JFloat seconds);

	void		ReadSetup(std::istream& input);
	static void	SkipSetup(std::istream& input);
	void		WriteSetup(std::ostream& output) const;

	void	PlaceScrolltab();
	void	ScrollToTab(const JIndex tabIndex);
	void	RemoveAllScrolltabs();

	bool	IsHorizontal() const;
	bool	IsVertical() const;

	// for use by JXScrollableWidgets

	void	PrepareForLowerMaxValue(const JCoordinate start, const JSize length);
	void	PrepareForHigherMaxValue(const JCoordinate start, const JSize length);
	void	PrepareForScaledMaxValue(const JFloat scaleFactor);
	void	StripMoved(const JCoordinate origStart, const JSize length,
					   const JCoordinate newStart);
	void	StripsSwapped(const JCoordinate start1, const JSize length1,
						  const JCoordinate start2, const JSize length2);

	// called by JXScrolltab

	void	ScrolltabCreated(JXScrolltab* tab);
	void	ScrolltabDeleted(JXScrolltab* tab);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum DragAction
	{
		kInvalidClick,
		kDecrementValue,
		kIncrementValue,
		kDecrementPage,
		kIncrementPage,
		kDragThumb
	};

private:

	JCoordinate	itsValue;
	JCoordinate	itsStepSize;
	JCoordinate	itsPageStepSize;
	JCoordinate	itsMaxValue;
	JFloat		itsContScrollDelay;

	JRect	itsDecrArrowRect[2];	// [0] is always visible, [1] if there is space
	JRect	itsIncrArrowRect[2];
	JRect	itsThumbRect;
	JRect	itsThumbDragRect;
	JRect	itsDecrementPageRect;
	JRect	itsIncrementPageRect;

	JPtrArray<JXScrolltab>*	itsScrolltabList;				// can be nullptr
	bool				itsIgnoreScrolltabDeletedFlag;

	JXTextMenu*	itsActionMenu;		// can be nullptr

	JCursorIndex	itsSpeedScrollCursor;

	// used during dragging

	DragAction	itsDragAction;
	JPoint		itsDragThumbOffset;
	bool	itsDecrPushedFlag;
	bool	itsIncrPushedFlag;
	bool	itsDecrPagePushedFlag;
	bool	itsIncrPagePushedFlag;

	// used for context menu

	JCoordinate	itsHereValue;

private:

	void	DrawHoriz(JPainter& p);
	void	DrawVert(JPainter& p);

	JCoordinate	ClickToValue(const JPoint& pt) const;
	JCoordinate	ClickToValueHoriz(const JPoint& pt) const;
	JCoordinate	ClickToValueVert(const JPoint& pt) const;

	void	ScrollWait(const JFloat delta) const;

	JCursorIndex	GetSpeedScrollCursor();
	void			StepArrow(const JCoordinate count,
							  const JXKeyModifiers& modifiers);

	void	OpenActionMenu(const JPoint& pt, const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers);
	void	HandleActionMenu(const JIndex index);

	static JListT::CompareResult
		CompareScrolltabValues(JXScrolltab* const & t1, JXScrolltab* const & t2);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kScrolled;

	class Scrolled : public JBroadcaster::Message
	{
	public:

		Scrolled(const JCoordinate value)
			:
			JBroadcaster::Message(kScrolled),
			itsValue(value)
			{ };

		JCoordinate
		GetValue() const
		{
			return itsValue;
		};

	private:

		JCoordinate	itsValue;
	};
};

/******************************************************************************
 GetValue

 ******************************************************************************/

inline JCoordinate
JXScrollbar::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 AdjustValue

 ******************************************************************************/

inline void
JXScrollbar::AdjustValue
	(
	const JCoordinate delta
	)
{
	SetValue(itsValue + delta);
}

/******************************************************************************
 StepLine

 ******************************************************************************/

inline void
JXScrollbar::StepLine
	(
	const JCoordinate lineCount
	)
{
	AdjustValue(lineCount * itsStepSize);
}

/******************************************************************************
 StepPage

 ******************************************************************************/

inline void
JXScrollbar::StepPage
	(
	const JFloat pageCount
	)
{
	AdjustValue(JRound(pageCount * itsPageStepSize));
}

/******************************************************************************
 IsAtMin

 ******************************************************************************/

inline bool
JXScrollbar::IsAtMin()
{
	return itsValue == kMinValue;
}

/******************************************************************************
 ScrollToMin

 ******************************************************************************/

inline void
JXScrollbar::ScrollToMin()
{
	SetValue(kMinValue);
}

/******************************************************************************
 IsAtMax

 ******************************************************************************/

inline bool
JXScrollbar::IsAtMax()
{
	return itsValue == itsMaxValue;
}

/******************************************************************************
 ScrollToMax

 ******************************************************************************/

inline void
JXScrollbar::ScrollToMax()
{
	SetValue(itsMaxValue);
}

/******************************************************************************
 GetStepSize

 ******************************************************************************/

inline JCoordinate
JXScrollbar::GetStepSize()
	const
{
	return itsStepSize;
}

/******************************************************************************
 SetStepSize

 ******************************************************************************/

inline void
JXScrollbar::SetStepSize
	(
	const JCoordinate step
	)
{
	if (step > 0)
		{
		itsStepSize = step;
		}
	else
		{
		itsStepSize = 1;
		}
}

/******************************************************************************
 GetPageStepSize

 ******************************************************************************/

inline JCoordinate
JXScrollbar::GetPageStepSize()
	const
{
	return itsPageStepSize;
}

/******************************************************************************
 SetPageStepSize

 ******************************************************************************/

inline void
JXScrollbar::SetPageStepSize
	(
	const JCoordinate step
	)
{
	if (step > 0)
		{
		itsPageStepSize = step;
		}
	else
		{
		itsPageStepSize = 1;
		}
}

/******************************************************************************
 GetMaxValue

 ******************************************************************************/

inline JCoordinate
JXScrollbar::GetMaxValue()
	const
{
	return itsMaxValue;
}

/******************************************************************************
 Scroll delay

	This specifies how long to wait between actions when the mouse
	is held down on the arrows.

 ******************************************************************************/

inline JFloat
JXScrollbar::GetScrollDelay()
	const
{
	return itsContScrollDelay;
}

inline void
JXScrollbar::SetScrollDelay
	(
	const JFloat seconds
	)
{
	itsContScrollDelay = seconds;
}

/******************************************************************************
 Orientation

 ******************************************************************************/

inline bool
JXScrollbar::IsHorizontal()
	const
{
	const JRect frameG = GetFrameGlobal();
	return frameG.width() > frameG.height();
}

inline bool
JXScrollbar::IsVertical()
	const
{
	return !IsHorizontal();
}

#endif
