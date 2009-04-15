/******************************************************************************
 JXWidget.h

	Interface for the JXWidget class

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWidget
#define _H_JXWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXContainer.h>
#include <JXSelectionManager.h>		// need defn of Data
#include <JXDNDManager.h>			// need defn of TargetFinder
#include <jColor.h>

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

	virtual void	Deactivate();

	virtual void	Refresh() const;
	void			RefreshRect(const JRect& rect) const;
	virtual void	Redraw() const;
	void			RedrawRect(const JRect& rect) const;

	JBoolean			Focus();
	JBoolean			Unfocus();
	JBoolean			HasFocus() const;
	JBoolean			WillAcceptFocus() const;
	void				SetSingleFocusWidget();
	virtual JBoolean	OKToUnfocus();
	JBoolean			WantsTab() const;
	JBoolean			WantsModifiedTab() const;
	virtual void		HandleKeyPress(const int key,				// must call inherited
									   const JXKeyModifiers& modifiers);

	JBoolean		WillAcceptShortcut() const;
	virtual void	HandleShortcut(const int key,					// must call inherited
								   const JXKeyModifiers& modifiers);

	virtual JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const;
	virtual JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const;

	virtual void	Place(const JCoordinate enclX, const JCoordinate enclY);
	virtual void	Move(const JCoordinate dx, const JCoordinate dy);
	virtual void	SetSize(const JCoordinate w, const JCoordinate h);
	virtual void	AdjustSize(const JCoordinate dw, const JCoordinate dh);
	HSizingOption	GetHSizing() const;
	VSizingOption	GetVSizing() const;
	void			SetSizing(const HSizingOption hSizing, const VSizingOption vSizing);

	void	CenterWithinEnclosure(const JBoolean adjustHoriz, const JBoolean adjustVert);
	void	FitToEnclosure(const JBoolean fitHoriz = kJTrue, const JBoolean fitVert = kJTrue);

	JBoolean	Scroll(const JCoordinate dx, const JCoordinate dy);
	JBoolean	ScrollTo(const JCoordinate x, const JCoordinate y);
	JBoolean	ScrollTo(const JPoint& pt);
	JBoolean	ScrollToRect(const JRect& r);
	JBoolean	ScrollToRectCentered(const JRect& r, const JBoolean forceScroll);

	virtual JRect		GetBoundsGlobal() const;
	virtual JRect		GetFrameGlobal() const;
	virtual JRect		GetApertureGlobal() const;
	JSize				GetBorderWidth() const;
	void				SetBorderWidth(const JSize width);

	// background color

	JColorIndex	GetCurrBackColor() const;
	JColorIndex	GetBackColor() const;
	void		SetBackColor(const JColorIndex color);
	JColorIndex	GetFocusColor() const;
	void		SetFocusColor(const JColorIndex color);

protected:

	JXWidget(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	void	WantInput(const JBoolean wantInput,
					  const JBoolean wantTab = kJFalse,
					  const JBoolean wantModifiedTab = kJFalse);

	virtual void	HandleFocusEvent();			// must call inherited
	virtual void	HandleUnfocusEvent();		// must call inherited

	virtual void	HandleWindowFocusEvent();	// must call inherited
	virtual void	HandleWindowUnfocusEvent();	// must call inherited

	void	UnlockBounds();
	void	SetBounds(const JCoordinate w, const JCoordinate h);
	void	AdjustBounds(const JCoordinate dw, const JCoordinate dh);

	virtual void		DrawBackground(JXWindowPainter& p, const JRect& frame);
	virtual JBoolean	AcceptDrag(const JPoint& pt, const JXMouseButton button,
								   const JXKeyModifiers& modifiers);

	// drawing during a mouse drag

	JXDragPainter*	CreateDragInsidePainter();
	JXDragPainter*	CreateDragOutsidePainter();
	JBoolean		GetDragPainter(JXDragPainter** p) const;
	JBoolean		GetDragPainter(JPainter** p) const;
	void			DeleteDragPainter();

	// position and size adjustments -- must call inherited

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	ApertureMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

	// Drag-And-Drop routines

	JBoolean		BeginDND(const JPoint& pt, const JXButtonStates& buttonStates,
							 const JXKeyModifiers& modifiers,
							 JXSelectionData* data,
							 JXDNDManager::TargetFinder* targetFinder = NULL);
	virtual void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);
	virtual void	GetSelectionData(JXSelectionData* data, const JCharacter* id);
	virtual void	DNDFinish(const JBoolean isDrop, const JXContainer* target);
	virtual void	DNDCompletelyFinished();
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual void	GetDNDAskActions(const JXButtonStates& buttonStates,
									 const JXKeyModifiers& modifiers,
									 JArray<Atom>* askActionList,
									 JPtrArray<JString>* askDescriptionList);
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);

private:

	JRect		itsFrameG;				// global coords
	JRect		itsBoundsG;				// global coords
	JBoolean	itsApertureBoundedFlag;	// kJTrue if we are bounded by aperture
	JSize		itsBorderWidth;
	// aperture is calculated when needed

	// resizing information

	HSizingOption	itsHSizing;
	VSizingOption	itsVSizing;

	// focus information

	JBoolean	itsWantInputFlag;
	JBoolean	itsWantTabFlag;
	JBoolean	itsWantModTabFlag;

	// background colors

	JColorIndex	itsBackColor;
	JColorIndex	itsFocusColor;

	// Used to draw during dragging.  We need a pointer
	// to it so we can update its origin after scrolling

	JXDragPainter*	itsDragPainter;

private:

	JXDragPainter*	CreateDragPainter(const JXContainer* widget);
	JBoolean		KeepApertureInsideBounds(JCoordinate* dx, JCoordinate* dy) const;

	// called by JXWindow

	void	Focus(const int x);
	void	NotifyFocusLost();

	// not allowed

	JXWidget(const JXWidget& source);
	const JXWidget& operator=(const JXWidget& source);

public:

	// JBroadcaster messages

	static const JCharacter* kGotFocus;
	static const JCharacter* kLostFocus;

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

inline JBoolean
JXWidget::GetDragPainter
	(
	JXDragPainter** p
	)
	const
{
	*p = itsDragPainter;
	return JI2B(itsDragPainter != NULL);
}

/******************************************************************************
 UnlockBounds (protected)

	Clear the flag so Bounds is no longer locked to Aperture.

 ******************************************************************************/

inline void
JXWidget::UnlockBounds()
{
	itsApertureBoundedFlag = kJFalse;
}

/******************************************************************************
 ScrollTo

 ******************************************************************************/

inline JBoolean
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

inline JBoolean
JXWidget::WantsTab()
	const
{
	return itsWantTabFlag;
}

inline JBoolean
JXWidget::WantsModifiedTab()
	const
{
	return itsWantModTabFlag;
}

/******************************************************************************
 Colors

 ******************************************************************************/

inline JColorIndex
JXWidget::GetCurrBackColor()
	const
{
	return (HasFocus() ? itsFocusColor : itsBackColor);
}

inline JColorIndex
JXWidget::GetBackColor()
	const
{
	return itsBackColor;
}

inline void
JXWidget::SetBackColor
	(
	const JColorIndex color
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

inline JColorIndex
JXWidget::GetFocusColor()
	const
{
	return itsFocusColor;
}

inline void
JXWidget::SetFocusColor
	(
	const JColorIndex color
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

	Returns kJTrue if the widget will accept a Focus() message in its
	current state.

 ******************************************************************************/

inline JBoolean
JXWidget::WillAcceptFocus()
	const
{
	return JConvertToBoolean( itsWantInputFlag && IsVisible() && IsActive() );
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

	Returns kJTrue if the widget will accept a HandleShortcut() message in its
	current state.

 ******************************************************************************/

inline JBoolean
JXWidget::WillAcceptShortcut()
	const
{
	return JConvertToBoolean( IsVisible() && IsActive() );
}

#endif
