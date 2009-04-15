/*********************************************************************************
 JX2DPlotWidget.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#ifndef _H_JX2DPlotWidget
#define _H_JX2DPlotWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>
#include <J2DPlotWidget.h>

class JXPSPrinter;
class JX2DPlotEPSPrinter;
class JXTextMenu;
class JXMenuBar;
class JXFontNameMenu;
class JXFontSizeMenu;
class JX2DPlotLabelDialog;
class JX2DPlotScaleDialog;
class JX2DPlotRangeDialog;
class JX2DCurveOptionsDialog;
class JX2DCursorMarkTableDir;

class JX2DPlotWidget : public JXWidget, public J2DPlotWidget
{
public:

	JX2DPlotWidget(JXMenuBar* menuBar, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JX2DPlotWidget(JX2DPlotWidget* plot, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JX2DPlotWidget();

	JXTextMenu*		GetCursorMenu() const;
	JXTextMenu*		GetOptionsMenu() const;
	JXTextMenu*		GetCurveOptionsMenu() const;
	JIndex			GetCurveOptionsMenuCurveIndex() const;

	void			SetPSPrinter(JXPSPrinter* p);
	const JString&	GetPSPrintFileName() const;
	void			SetPSPrintFileName(const JCharacter* fileName);
	void			HandlePSPageSetup();
	void			PrintPS();

	void			SetEPSPrinter(JX2DPlotEPSPrinter* p);
	const JString&	GetEPSPlotFileName() const;
	void			SetEPSPlotFileName(const JCharacter* fileName);
	const JRect&	GetEPSPlotBounds() const;
	void			SetEPSPlotBounds(const JRect& rect);
	void			PrintPlotEPS();
	const JString&	GetEPSMarksFileName() const;
	void			SetEPSMarksFileName(const JCharacter* fileName);
	void			PrintMarksEPS();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	void	PWXReadSetup(istream& input);
	void	PWXWriteSetup(ostream& output) const;

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);

	virtual JCoordinate	GetMarksHeight() const;
	virtual JBoolean	PrintMarks(JPagePrinter& p, const JBoolean putOnSamePage,
									const JRect& partialPageRect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);

	virtual JSize	PWGetGUIWidth() const;
	virtual JSize	PWGetGUIHeight() const;

	virtual JPainter*	PWCreateDragInsidePainter();
	virtual JBoolean	PWGetDragPainter(JPainter** p) const;
	virtual void		PWDeleteDragPainter();

	virtual void	PWRefreshRect(const JRect& rect);
	virtual void	PWRedraw();
	virtual void	PWRedrawRect(const JRect& rect);
	virtual void	PWForceRefresh();
	virtual void	PWDisplayCursor(const MouseCursor cursor);

	virtual void	ProtectionChanged();
	virtual void	ChangeCurveOptions(const JIndex index);
	virtual void	ChangeLabels(const LabelSelection selection);
	virtual void	ChangeScale(const JBoolean xAxis);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu*				itsOptionsMenu;
	JXTextMenu*				itsRemoveCurveMenu;
	JXTextMenu*				itsCursorMenu;
	JXTextMenu*				itsMarkMenu;
	JX2DPlotLabelDialog*	itsPlotLabelDialog;
	JX2DPlotScaleDialog*	itsPlotScaleDialog;
	JX2DPlotRangeDialog*	itsPlotRangeDialog;
	JX2DCurveOptionsDialog*	itsCurveOptionsDialog;
	JIndex					itsCurrentCurve;
	JPoint					itsStartPt;
	JPoint					itsPrevPt;
	JX2DCursorMarkTableDir*	itsMarkDir;
	JCursorIndex			itsDragXCursor;
	JCursorIndex			itsDragYCursor;

	JXTextMenu*				itsCurveOptionsMenu;
	JIndex					itsCurveOptionsIndex;

	JXPSPrinter*			itsPSPrinter;		// not owned; can be NULL
	JString					itsPSPrintName;		// file name when printing to a file

	JX2DPlotEPSPrinter*		itsEPSPrinter;		// not owned; can be NULL
	JString					itsEPSPlotName;		// output file name
	JRect					itsEPSPlotBounds;	// bounding rect when printed
	JString					itsEPSMarksName;	// output file name
	JBoolean				itsPrintEPSPlotFlag;

private:

	void JX2DPlotWidgetX();

	void UpdateOptionsMenu();
	void HandleOptionsMenu(const JIndex index);

	void UpdateCurvesMenu();
	void HandleCurvesMenu(const JIndex index);

	void UpdateRemoveCurveMenu();
	void HandleRemoveCurveMenu(const JIndex index);

	void UpdateCursorMenu();
	void HandleCursorMenu(const JIndex index);

	void UpdateMarkMenu();
	void HandleMarkMenu(const JIndex index);

	void UpdateCurveOptionsMenu();
	void HandleCurveOptionsMenu(const JIndex index);

	void GetNewLabels();

	void ChangeScale();
	void GetNewScale();

	void ChangeRange();
	void GetNewRange();

	void ChangeCurveOptions();
	void GetNewCurveOptions();

	// not allowed

	JX2DPlotWidget(const JX2DPlotWidget& source);
	const JX2DPlotWidget& operator=(const JX2DPlotWidget& source);
};


/******************************************************************************
 GetCursorMenu

 ******************************************************************************/

inline JXTextMenu*
JX2DPlotWidget::GetCursorMenu()
	const
{
	return itsCursorMenu;
}

/*******************************************************************************
 GetOptionsMenu

 ******************************************************************************/

inline JXTextMenu*
JX2DPlotWidget::GetOptionsMenu()
	const
{
	return itsOptionsMenu;
}

/*******************************************************************************
 GetCurveOptionsMenu

 ******************************************************************************/

inline JXTextMenu*
JX2DPlotWidget::GetCurveOptionsMenu()
	const
{
	return itsCurveOptionsMenu;
}

/*******************************************************************************
 GetCurveOptionsMenuCurveIndex

	Returns index of curve to which menu currently applies.

 ******************************************************************************/

inline JIndex
JX2DPlotWidget::GetCurveOptionsMenuCurveIndex()
	const
{
	return itsCurveOptionsIndex;
}

/******************************************************************************
 PS print file name

	Call this to get/set the file name used in the Print Setup dialog.

 ******************************************************************************/

inline const JString&
JX2DPlotWidget::GetPSPrintFileName()
	const
{
	return itsPSPrintName;
}

inline void
JX2DPlotWidget::SetPSPrintFileName
	(
	const JCharacter* fileName
	)
{
	itsPSPrintName = fileName;
}

/******************************************************************************
 EPS plot file name

	Call this to get/set the file name used in the Print Setup dialog.

 ******************************************************************************/

inline const JString&
JX2DPlotWidget::GetEPSPlotFileName()
	const
{
	return itsEPSPlotName;
}

inline void
JX2DPlotWidget::SetEPSPlotFileName
	(
	const JCharacter* fileName
	)
{
	itsEPSPlotName = fileName;
}

/******************************************************************************
 EPS plot bounds

	Call this to get the initial bounds used in the Print Setup dialog.

 ******************************************************************************/

inline const JRect&
JX2DPlotWidget::GetEPSPlotBounds()
	const
{
	return itsEPSPlotBounds;
}

inline void
JX2DPlotWidget::SetEPSPlotBounds
	(
	const JRect& rect
	)
{
	itsEPSPlotBounds = rect;
}

/******************************************************************************
 GetEPSMarksFileName

	Call this to get the file name used in the Print Setup dialog.

 ******************************************************************************/

inline const JString&
JX2DPlotWidget::GetEPSMarksFileName()
	const
{
	return itsEPSMarksName;
}

/******************************************************************************
 SetEPSMarksFileName

	Call this to set the file name used in the Print Setup dialog.

 ******************************************************************************/

inline void
JX2DPlotWidget::SetEPSMarksFileName
	(
	const JCharacter* fileName
	)
{
	itsEPSMarksName = fileName;
}

#endif
