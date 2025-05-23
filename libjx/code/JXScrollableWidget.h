/******************************************************************************
 JXScrollableWidget.h

	Interface for the JXScrollableWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXScrollableWidget
#define _H_JXScrollableWidget

#include "JXWidget.h"

class JXScrollbar;
class JXScrollbarSet;
class JXUrgentTask;

class JXScrollableWidget : public JXWidget
{
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

	~JXScrollableWidget() override;

	void	AlwaysShowScrollbars(const bool alwaysShow = true);
	bool	GetScrollbars(JXScrollbar** hScrollbar, JXScrollbar** vScrollbar) const;
	void	UpdateScrollbars();

	void		ReadScrollSetup(std::istream& input);
	static void	SkipScrollSetup(std::istream& input);
	void		WriteScrollSetup(std::ostream& output) const;

	void	HandleKeyPress(const JUtf8Character& c, const int key,
						   const JXKeyModifiers& modifiers) override;

	DisplayState	SaveDisplayState() const;
	void			RestoreDisplayState(const DisplayState& state);

protected:

	bool	ScrollForDrag(const JPoint& pt);
	bool	ScrollForWheel(const JXMouseButton button,
						   const JXKeyModifiers& modifiers);
	bool	ScrollForWheel(const JXMouseButton button,
						   const JXKeyModifiers& modifiers,
						   JXScrollbar* hScrollbar, JXScrollbar* vScrollbar);

	void	SetHorizStepSize(const JCoordinate hStep);
	void	SetHorizPageStepContext(const JCoordinate hPageContext);
	void	SetVertStepSize(const JCoordinate vStep);
	void	SetVertPageStepContext(const JCoordinate vPageContext);

	void	SetHorizJumpToScrolltabModifiers(const bool ctrl, const bool meta);
	void	SetVertJumpToScrolltabModifiers(const bool ctrl, const bool meta);

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	void	ApertureMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	void	HandleDNDScroll(const JPoint& pt, const JXMouseButton scrollButton,
							const JXKeyModifiers& modifiers) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXScrollbarSet*	itsScrollbarSet;		// can be nullptr
	bool			itsAlwaysShowScrollFlag;
	bool			itsAdjustingFlag;		// if should ignore scrollbar messages
	bool			itsShouldRedrawFlag;	// if Redraw() when scrolled

	JXUrgentTask*	itsAdjustScrollbarTask;

	// If these are positive, they override the defaults.

	JCoordinate	itsHStepSize;
	JCoordinate	itsVStepSize;

	// If these are non-negative, they override the defaults.

	JCoordinate	itsHPageStepContext;
	JCoordinate	itsVPageStepContext;

	// modifiers for jumping to scrolltab -- feature turned off if both false

	bool	itsHCtrl, itsHMeta;
	bool	itsVCtrl, itsVMeta;

private:

	void	NeedAdjustScrollbars();
	void	AdjustScrollbars();
	JFloat	GetSingleStepFraction() const;
};


/******************************************************************************
 AlwaysShowScrollbars

 ******************************************************************************/

inline void
JXScrollableWidget::AlwaysShowScrollbars
	(
	const bool alwaysShow
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
	const bool ctrl,
	const bool meta
	)
{
	itsHCtrl = ctrl;
	itsHMeta = meta;
}

inline void
JXScrollableWidget::SetVertJumpToScrolltabModifiers
	(
	const bool ctrl,
	const bool meta
	)
{
	itsVCtrl = ctrl;
	itsVMeta = meta;
}

#endif
