/******************************************************************************
 JXWidget.h

	Interface for the JXWidget class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWidget
#define _H_JXWidget

#include "jx-af/jx/JXContainer.h"
#include "jx-af/jx/JXSelectionManager.h"		// need defn of Data
#include "jx-af/jx/JXDNDManager.h"			// need defn of TargetFinder
#include <jx-af/jcore/jColor.h>

class JString;
class JPainter;
class JXDragPainter;

class JXWidget : public JXContainer
{
	friend class JXWindow;
	friend class JXDNDManager;
	friend class JXSelectionData;

public:

	enum HSizingOption
	{
		kFixedLeft,
		kFixedRight,
		kHElastic
	};

	enum VSizingOption
	{
		kFixedTop,
		kFixedBottom,
		kVElastic
	};

public:

	virtual ~JXWidget();

	virtual void	Deactivate() override;

	virtual void	Refresh() const override;
	void			RefreshRect(const JRect& rect) const;
	virtual void	Redraw() const override;
	void			RedrawRect(const JRect& rect) const;

	bool			Focus();
	bool			Unfocus();
	bool			HasFocus() const;
	bool			WillAcceptFocus() const;
	void			SetSingleFocusWidget();
	virtual bool	OKToUnfocus();
	bool			WantsTab() const;
	bool			WantsModifiedTab() const;
	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,	// must call inherited
								   const JXKeyModifiers& modifiers);

	bool			WillAcceptShortcut() const;
	virtual void	HandleShortcut(const int key,					// must call inherited
								   const JXKeyModifiers& modifiers);

	virtual JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const override;
	virtual JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const override;

	virtual void	Place(const JCoordinate enclX, const JCoordinate enclY) override;
	virtual void	Move(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	SetSize(const JCoordinate w, const JCoordinate h) override;
	virtual void	AdjustSize(const JCoordinate dw, const JCoordinate dh) override;
	HSizingOption	GetHSizing() const;
	VSizingOption	GetVSizing() const;
	void			SetSizing(const HSizingOption hSizing, const VSizingOption vSizing);

	void	CenterWithinEnclosure(const bool adjustHoriz, const bool adjustVert);
	void	FitToEnclosure(const bool fitHoriz = true, const bool fitVert = true);

	bool	Scroll(const JCoordinate dx, const JCoordinate dy);
	bool	ScrollTo(const JCoordinate x, const JCoordinate y);
	bool	ScrollTo(const JPoint& pt);
	bool	ScrollToRect(const JRect& r);
	bool	ScrollToRectCentered(const JRect& r, const bool forceScroll);

	virtual JRect	GetBoundsGlobal() const override;
	virtual JRect	GetFrameGlobal() const override;
	virtual JRect	GetApertureGlobal() const override;
	JSize			GetBorderWidth() const;
	void			SetBorderWidth(const JSize width);

	// background color

	JColorID	GetCurrBackColor() const;
	JColorID	GetBackColor() const;
	void		SetBackColor(const JColorID color);
	JColorID	GetFocusColor() const;
	void		SetFocusColor(const JColorID color);

	virtual JString	ToString() const override;

protected:

	JXWidget(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	void	WantInput(const bool wantInput,
					  const bool wantTab = false,
					  const bool wantModifiedTab = false);

	virtual void	HandleFocusEvent();			// must call inherited
	virtual void	HandleUnfocusEvent();		// must call inherited

	virtual void	HandleWindowFocusEvent();	// must call inherited
	virtual void	HandleWindowUnfocusEvent();	// must call inherited

	void	UnlockBounds();
	void	SetBounds(const JCoordinate w, const JCoordinate h);
	void	AdjustBounds(const JCoordinate dw, const JCoordinate dh);

	virtual void	DrawBackground(JXWindowPainter& p, const JRect& frame) override;
	virtual bool	AcceptDrag(const JPoint& pt, const JXMouseButton button,
							   const JXKeyModifiers& modifiers) override;

	// drawing during a mouse drag

	JXDragPainter*	CreateDragInsidePainter();
	JXDragPainter*	CreateDragOutsidePainter();
	bool			GetDragPainter(JXDragPainter** p) const;
	bool			GetDragPainter(JPainter** p) const;
	void			DeleteDragPainter();

	// position and size adjustments -- must call inherited

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	ApertureMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

	virtual void	FTCAdjustSize(const JCoordinate dw, const JCoordinate dh) override;

	// Drag-And-Drop routines

	bool			BeginDND(const JPoint& pt, const JXButtonStates& buttonStates,
							 const JXKeyModifiers& modifiers,
							 JXSelectionData* data,
							 JXDNDManager::TargetFinder* targetFinder = nullptr);
	virtual void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);
	virtual void	GetSelectionData(JXSelectionData* data, const JString& id);
	virtual void	DNDFinish(const bool isDrop, const JXContainer* target);
	virtual void	DNDCompletelyFinished();
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual void	GetDNDAskActions(const JXButtonStates& buttonStates,
									 const JXKeyModifiers& modifiers,
									 JArray<Atom>* askActionList,
									 JPtrArray<JString>* askDescriptionList);
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const bool dropAccepted, const Atom action);

	// use with extreme caution

	bool	WillAllowUnboundedScrolling() const;
	void	ShouldAllowUnboundedScrolling(const bool allow);

private:

	JRect	itsFrameG;				// global coords
	JRect	itsBoundsG;				// global coords
	bool	itsApertureBoundedFlag;	// true if we are bounded by aperture
	JSize	itsBorderWidth;
	bool	itsAllowUnboundedScrollingFlag;	// true if scroll offset is not restricted
	// aperture is calculated when needed

	// resizing information

	HSizingOption	itsHSizing;
	VSizingOption	itsVSizing;

	// focus information

	bool	itsWantInputFlag;
	bool	itsWantTabFlag;
	bool	itsWantModTabFlag;

	// background colors

	JColorID	itsBackColor;
	JColorID	itsFocusColor;

	// Used to draw during dragging.  We need a pointer
	// to it so we can update its origin after scrolling

	JXDragPainter*	itsDragPainter;

private:

	JXDragPainter*	CreateDragPainter(const JXContainer* widget);
	bool			KeepApertureInsideBounds(JCoordinate* dx, JCoordinate* dy) const;

	// called by JXWindow

	void	Focus(const int x);
	void	NotifyFocusLost();

public:

	// JBroadcaster messages

	static const JUtf8Byte* kGotFocus;
	static const JUtf8Byte* kLostFocus;

	class GotFocus : public JBroadcaster::Message
		{
		public:

			GotFocus()
				:
				JBroadcaster::Message(kGotFocus)
				{ };
		};

	class LostFocus : public JBroadcaster::Message
		{
		public:

			LostFocus()
				:
				JBroadcaster::Message(kLostFocus)
				{ };
		};
};


/******************************************************************************
 GetBorderWidth

 ******************************************************************************/

inline JSize
JXWidget::GetBorderWidth()
	 const
{
	return itsBorderWidth;
}

/******************************************************************************
 CreateDragInsidePainter (protected)

	Create a DragPainter that works inside our aperture.

 ******************************************************************************/

inline JXDragPainter*
JXWidget::CreateDragInsidePainter()
{
	return CreateDragPainter(this);
}

/******************************************************************************
 CreateDragOutsidePainter (protected)

	Create a DragPainter that works inside our enclosure's aperture.

 ******************************************************************************/

inline JXDragPainter*
JXWidget::CreateDragOutsidePainter()
{
	return CreateDragPainter(GetEnclosure());
}

/******************************************************************************
 GetDragPainter (protected)

 ******************************************************************************/

inline bool
JXWidget::GetDragPainter
	(
	JXDragPainter** p
	)
	const
{
	*p = itsDragPainter;
	return itsDragPainter != nullptr;
}

/******************************************************************************
 UnlockBounds (protected)

	Clear the flag so Bounds is no longer locked to Aperture.

 ******************************************************************************/

inline void
JXWidget::UnlockBounds()
{
	itsApertureBoundedFlag = false;
}

/******************************************************************************
 ScrollTo

 ******************************************************************************/

inline bool
JXWidget::ScrollTo
	(
	const JPoint& pt
	)
{
	return ScrollTo(pt.x, pt.y);
}

/******************************************************************************
 Sizing options

 ******************************************************************************/

inline JXWidget::HSizingOption
JXWidget::GetHSizing()
	const
{
	return itsHSizing;
}

inline JXWidget::VSizingOption
JXWidget::GetVSizing()
	const
{
	return itsVSizing;
}

inline void
JXWidget::SetSizing
	(
	const HSizingOption hSizing,
	const VSizingOption vSizing
	)
{
	itsHSizing = hSizing;
	itsVSizing = vSizing;
}

/******************************************************************************
 WantsTab

 ******************************************************************************/

inline bool
JXWidget::WantsTab()
	const
{
	return itsWantTabFlag;
}

inline bool
JXWidget::WantsModifiedTab()
	const
{
	return itsWantModTabFlag;
}

/******************************************************************************
 Colors

 ******************************************************************************/

inline JColorID
JXWidget::GetCurrBackColor()
	const
{
	return (HasFocus() ? itsFocusColor : itsBackColor);
}

inline JColorID
JXWidget::GetBackColor()
	const
{
	return itsBackColor;
}

inline void
JXWidget::SetBackColor
	(
	const JColorID color
	)
{
	if (color != itsBackColor)
		{
		itsBackColor = color;
		if (!HasFocus())
			{
			Refresh();
			}
		}
}

inline JColorID
JXWidget::GetFocusColor()
	const
{
	return itsFocusColor;
}

inline void
JXWidget::SetFocusColor
	(
	const JColorID color
	)
{
	if (color != itsFocusColor)
		{
		itsFocusColor = color;
		if (HasFocus())
			{
			Refresh();
			}
		}
}

/******************************************************************************
 WillAcceptFocus

	Returns true if the widget will accept a Focus() message in its
	current state.

 ******************************************************************************/

inline bool
JXWidget::WillAcceptFocus()
	const
{
	return itsWantInputFlag && IsVisible() && IsActive();
}

/******************************************************************************
 SetSingleFocusWidget

	Call this function if this widget will always be the only one in the
	window that accepts focus.

 ******************************************************************************/

inline void
JXWidget::SetSingleFocusWidget()
{
	SetFocusColor(itsBackColor);
}

/******************************************************************************
 WillAcceptShortcut

	Returns true if the widget will accept a HandleShortcut() message in its
	current state.

 ******************************************************************************/

inline bool
JXWidget::WillAcceptShortcut()
	const
{
	return IsVisible() && IsActive();
}

/******************************************************************************
 WillAllowUnboundedScrolling

	Unbounded scrolling does not constrain the scroll offset.

 ******************************************************************************/

inline bool
JXWidget::WillAllowUnboundedScrolling()
	const
{
	return itsAllowUnboundedScrollingFlag;
}

#endif
