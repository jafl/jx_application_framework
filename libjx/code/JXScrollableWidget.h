/******************************************************************************
 JXScrollableWidget.h

	Interface for the JXScrollableWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXScrollableWidget
#define _H_JXScrollableWidget

#include <JXWidget.h>

class JXScrollbar;
class JXScrollbarSet;
class JXAdjustScrollbarTask;

class JXScrollableWidget : public JXWidget
{
	friend class JXAdjustScrollbarTask;

public:

	struct DisplayState
	{
		JCoordinate	h;
		JCoordinate	hMax;
		JCoordinate	v;
		JCoordinate	vMax;
	};

public:

	JXScrollableWidget(JXScrollbarSet* scrollbarSet,
					   JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~JXScrollableWidget();

	void		AlwaysShowScrollbars(const JBoolean alwaysShow = kJTrue);
	JBoolean	GetScrollbars(JXScrollbar** hScrollbar, JXScrollbar** vScrollbar) const;
	void		UpdateScrollbars();

	void		ReadScrollSetup(std::istream& input);
	static void	SkipScrollSetup(std::istream& input);
	void		WriteScrollSetup(std::ostream& output) const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

	DisplayState	SaveDisplayState() const;
	void			RestoreDisplayState(const DisplayState& state);

protected:

	JBoolean	ScrollForDrag(const JPoint& pt);
	JBoolean	ScrollForWheel(const JXMouseButton button,
							   const JXKeyModifiers& modifiers);
	JBoolean	ScrollForWheel(const JXMouseButton button,
							   const JXKeyModifiers& modifiers,
							   JXScrollbar* hScrollbar, JXScrollbar* vScrollbar);

	void	SetHorizStepSize(const JCoordinate hStep);
	void	SetHorizPageStepContext(const JCoordinate hPageContext);
	void	SetVertStepSize(const JCoordinate vStep);
	void	SetVertPageStepContext(const JCoordinate vPageContext);

	void	SetHorizJumpToScrolltabModifiers(const JBoolean ctrl, const JBoolean meta);
	void	SetVertJumpToScrolltabModifiers(const JBoolean ctrl, const JBoolean meta);

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	ApertureMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	virtual void	HandleDNDScroll(const JPoint& pt, const JXMouseButton scrollButton,
									const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXScrollbarSet*	itsScrollbarSet;		// can be nullptr
	JBoolean		itsAlwaysShowScrollFlag;
	JBoolean		itsWindowFrozenFlag;
	JBoolean		itsAdjustingFlag;		// if should ignore scrollbar messages
	JBoolean		itsShouldRedrawFlag;	// if Redraw() when scrolled

	JXAdjustScrollbarTask*	itsAdjustScrollbarTask;

	// If these are positive, they override the defaults.

	JCoordinate	itsHStepSize;
	JCoordinate	itsVStepSize;

	// If these are non-negative, they override the defaults.

	JCoordinate	itsHPageStepContext;
	JCoordinate	itsVPageStepContext;

	// modifiers for jumping to scrolltab -- feature turned off if both kJFalse

	JBoolean	itsHCtrl, itsHMeta;
	JBoolean	itsVCtrl, itsVMeta;

private:

	void	NeedAdjustScrollbars();
	void	AdjustScrollbars();
	JFloat	GetSingleStepFraction() const;

	// not allowed

	JXScrollableWidget(const JXScrollableWidget& source);
	const JXScrollableWidget& operator=(const JXScrollableWidget& source);
};


/******************************************************************************
 AlwaysShowScrollbars

 ******************************************************************************/

inline void
JXScrollableWidget::AlwaysShowScrollbars
	(
	const JBoolean alwaysShow
	)
{
	itsAlwaysShowScrollFlag = alwaysShow;
	AdjustScrollbars();
}

/******************************************************************************
 Step sizes

 ******************************************************************************/

inline void
JXScrollableWidget::SetHorizStepSize
	(
	const JCoordinate hStep
	)
{
	itsHStepSize = hStep;
	AdjustScrollbars();
}

inline void
JXScrollableWidget::SetHorizPageStepContext
	(
	const JCoordinate hPageContext
	)
{
	itsHPageStepContext = hPageContext;
	AdjustScrollbars();
}

inline void
JXScrollableWidget::SetVertStepSize
	(
	const JCoordinate vStep
	)
{
	itsVStepSize = vStep;
	AdjustScrollbars();
}

inline void
JXScrollableWidget::SetVertPageStepContext
	(
	const JCoordinate vPageContext
	)
{
	itsVPageStepContext = vPageContext;
	AdjustScrollbars();
}

/******************************************************************************
 Modifiers for jumping to scrolltab

	Modifiers + [1-9] jump to a scrolltab.

 ******************************************************************************/

inline void
JXScrollableWidget::SetHorizJumpToScrolltabModifiers
	(
	const JBoolean ctrl,
	const JBoolean meta
	)
{
	itsHCtrl = ctrl;
	itsHMeta = meta;
}

inline void
JXScrollableWidget::SetVertJumpToScrolltabModifiers
	(
	const JBoolean ctrl,
	const JBoolean meta
	)
{
	itsVCtrl = ctrl;
	itsVMeta = meta;
}

#endif
