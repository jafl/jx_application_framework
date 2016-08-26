/******************************************************************************
 JX3DWidget.h

	Interface for the JX3DWidget class

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX3DWidget
#define _H_JX3DWidget

#include <JXWidget.h>
#include <JVector.h>

class J3DCamera;

class JX3DWidget : public JXWidget
{
public:

	JX3DWidget(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JX3DWidget();

	J3DCamera*	GetCamera() const;
	void		SetCamera(J3DCamera* camera);

	JBoolean	WillShowFocus() const;
	void		ShouldShowFocus(const JBoolean show);

protected:

	enum DragType
	{
		kInvalidDrag,
		kRotateDrag
//		kShiftDrag
//		kZoomDrag
	};

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void	DrawBackground(JXWindowPainter& p, const JRect& frame);

	virtual void	HandleFocusEvent();
	virtual void	HandleUnfocusEvent();

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);

	void	BeginDrag(const DragType type, const JPoint& pt);
	void	ContinueDrag(const JPoint& pt);
	void	FinishDrag();

	JBoolean	ZoomForWheel(const JXMouseButton button, const JXKeyModifiers& modifiers);

	JBoolean	HandleRotationKeyPress(const int key, const JXKeyModifiers& modifiers);

private:

	J3DCamera*	itsCamera;
	JBoolean	itsShowFocusFlag;

	DragType	itsDragType;
	JPoint		itsStartPt;
	JVector		itsStartPos;

private:

	// not allowed

	JX3DWidget(const JX3DWidget& source);
	const JX3DWidget& operator=(const JX3DWidget& source);
};


/******************************************************************************
 Camera

 ******************************************************************************/

inline J3DCamera*
JX3DWidget::GetCamera()
	const
{
	return itsCamera;
}

inline void
JX3DWidget::SetCamera
	(
	J3DCamera* camera
	)
{
	itsCamera = camera;
}

/******************************************************************************
 Show focus via border

 ******************************************************************************/

inline JBoolean
JX3DWidget::WillShowFocus()
	const
{
	return itsShowFocusFlag;
}

inline void
JX3DWidget::ShouldShowFocus
	(
	const JBoolean show
	)
{
	itsShowFocusFlag = show;
}

#endif
