/*********************************************************************************
 JX2DPlotWidget.h

	Copyright (C) 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_JX2DPlotWidget
#define _H_JX2DPlotWidget

#include <jx-af/jx/JXWidget.h>
#include "J2DPlotWidget.h"

class JXPSPrinter;
class JX2DPlotEPSPrinter;
class JXTextMenu;
class JXMenuBar;
class JXFontNameMenu;
class JXFontSizeMenu;
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

	~JX2DPlotWidget() override;

	JXTextMenu*		GetCursorMenu() const;
	JXTextMenu*		GetOptionsMenu() const;
	JXTextMenu*		GetCurveOptionsMenu() const;
	JIndex			GetCurveOptionsMenuCurveIndex() const;

	void			SetPSPrinter(JXPSPrinter* p);
	const JString&	GetPSPrintFileName() const;
	void			SetPSPrintFileName(const JString& fileName);
	void			HandlePSPageSetup();
	void			PrintPS();

	void			SetEPSPrinter(JX2DPlotEPSPrinter* p);
	const JString&	GetEPSPlotFileName() const;
	void			SetEPSPlotFileName(const JString& fileName);
	const JRect&	GetEPSPlotBounds() const;
	void			SetEPSPlotBounds(const JRect& rect);
	void			PrintPlotEPS();
	const JString&	GetEPSMarksFileName() const;
	void			SetEPSMarksFileName(const JString& fileName);
	void			PrintMarksEPS();

	void		HandleKeyPress(const JUtf8Character& c, const int keySym,
							   const JXKeyModifiers& modifiers) override;

	void	PWXReadSetup(std::istream& input);
	void	PWXWriteSetup(std::ostream& output) const;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;

	JCoordinate	GetMarksHeight() const override;
	bool	PrintMarks(JPagePrinter& p, const bool putOnSamePage,
									const JRect& partialPageRect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;
	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	JSize	PWGetGUIWidth() const override;
	JSize	PWGetGUIHeight() const override;

	JPainter*	PWCreateDragInsidePainter() override;
	bool	PWGetDragPainter(JPainter** p) const override;
	void		PWDeleteDragPainter() override;

	void	PWRefreshRect(const JRect& rect) override;
	void	PWRedraw() override;
	void	PWRedrawRect(const JRect& rect) override;
	void	PWForceRefresh() override;
	void	PWDisplayCursor(const MouseCursor cursor) override;

	void	ProtectionChanged() override;
	void	ChangeCurveOptions(const JIndex index) override;
	void	ChangeLabels(const LabelSelection selection) override;
	void	ChangeScale(const bool editXAxis) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextMenu*				itsOptionsMenu;
	JXTextMenu*				itsRemoveCurveMenu;
	JXTextMenu*				itsCursorMenu;
	JXTextMenu*				itsMarkMenu;
	bool					itsIsSharingMenusFlag;
	JIndex					itsCurrentCurve;
	JPoint					itsStartPt;
	JPoint					itsPrevPt;
	JX2DCursorMarkTableDir*	itsMarkDir;
	JCursorIndex			itsDragXCursor;
	JCursorIndex			itsDragYCursor;

	JXTextMenu*				itsCurveOptionsMenu;
	JIndex					itsCurveOptionsIndex;

	JXPSPrinter*			itsPSPrinter;		// not owned; can be nullptr
	JString					itsPSPrintName;		// file name when printing to a file

	JX2DPlotEPSPrinter*		itsEPSPrinter;		// not owned; can be nullptr
	JString					itsEPSPlotName;		// output file name
	JRect					itsEPSPlotBounds;	// bounding rect when printed
	JString					itsEPSMarksName;	// output file name

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

	void ChangeScale();
	void ChangeRange();
	void ChangeCurveOptions();
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
	const JString& fileName
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
	const JString& fileName
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
	const JString& fileName
	)
{
	itsEPSMarksName = fileName;
}

#endif
