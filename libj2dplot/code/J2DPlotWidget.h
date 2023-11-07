/*********************************************************************************
 J2DPlotWidget.h

	Copyright (C) 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_J2DPlotWidget
#define _H_J2DPlotWidget

#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/jColor.h>
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JFontStyle.h>
#include "J2DCurveInfo.h"

class JPainter;
class JPagePrinter;
class JEPSPrinter;
class J2DPlotDataBase;
struct J2DDataPoint;
struct J2DVectorPoint;
struct J2DDataRect;

class J2DPlotWidget : virtual public JBroadcaster
{
public:

	enum
	{
		kMin = 0,
		kMax,
		kInc,
		kStart		// start of tick marks when "unclean" zoom
	};

	enum
	{
		kXMajor = 0,
		kXMinor,
		kYMajor,
		kYMinor
	};

	enum LabelSelection
	{
		kNoLabel = 1,
		kTitle,
		kYLabel,
		kXLabel
	};

	enum CursorIndex
	{
		kNoCursor = 0,
		kX1,
		kX2,
		kY1,
		kY2
	};

	enum MouseCursor
	{
		kDblClickChangeCursor,		// arrow
		kDragXCursor,				// drag horiz
		kDragYCursor,				// drag vert
		kZoomCursor					// crosshair
	};

	enum
	{
		kSigDigitCount = 5
	};

public:

	J2DPlotWidget(const JColorID black, const JColorID white,
				  const JColorID gray, const JColorID selection);

	~J2DPlotWidget() override;

	void	Print(JPagePrinter& p);
	void	DrawForPrint(JPagePrinter& p, const JRect& rect);
	void	Print(JEPSPrinter& p, const JRect& rect);

	static void	GetPSPortraitPrintSize(const JCoordinate pageWidth,
									   const JCoordinate pageHeight,
									   JCoordinate* w, JCoordinate* h);
	static void	GetPSLandscapePrintSize(const JCoordinate pageWidth,
										const JCoordinate pageHeight,
										JCoordinate* w, JCoordinate* h);

	void	PWReadSetup(std::istream& input);
	void	PWWriteSetup(std::ostream& output) const;

	void	PWReadCurveSetup(std::istream& input);
	void	PWWriteCurveSetup(std::ostream& output) const;

	bool	WillAutoRefresh() const;
	void	ShouldAutoRefresh(const bool refresh);

	bool	WillIgnoreCurveChanged() const;
	void	ShouldIgnoreCurveChanged(const bool ignore);

	void			SetLabels(const JString& title, const JString& xLabel, const JString& yLabel);
	const JString&	GetXLabel() const;
	void			SetXLabel(const JString& xLabel);
	const JString&	GetYLabel() const;
	void			SetYLabel(const JString& yLabel);
	const JString&	GetTitle() const;
	void			SetTitle(const JString& xLabel);


	const JString&	GetFontName() const;
	void			SetFontName(const JString& name);

	JSize			GetFontSize() const;
	void			SetFontSize(const JSize size);

	bool	LegendIsVisible() const;
	void	ShowLegend(const bool show = true);
	bool	FrameIsVisible() const;
	void	ShowFrame(const bool show = true);
	bool	GridIsVisible() const;
	void	ShowGrid(const bool show = true);

	bool	CurveIsVisible(const JIndex curveIndex) const;
	void	ShowCurve(const JIndex curveIndex, const bool show);
	void	ShowAllCurves();
	void	HideAllOtherCurves(const JIndex curveIndex);
	bool	LinesAreVisible(const JIndex curveIndex) const;
	void	ShowLines(const JIndex curveIndex, const bool show = true);
	bool	SymbolsAreVisible(const JIndex curveIndex) const;
	void	ShowSymbols(const JIndex curveIndex, const bool show = true);
	void	SetCurveStyle(const JIndex curveIndex, const bool lines,
						  const bool symbols);
	bool	CurveIsProtected(const JIndex curveIndex) const;
	void	ProtectCurve(const JIndex curveIndex, const bool protect = true);

	JSize	GetCurveCount() const;

	const JString&	GetCurveName(const JIndex index) const;
	void			SetCurveName(const JIndex index, const JString& name);

	void			AddColor(const JColorID color);
	JColorID		GetCurveColor(const JIndex index) const;
	J2DSymbolType	GetSymbolType(const JIndex index) const;

	J2DPlotDataBase*		GetCurve(const JIndex index);
	const J2DPlotDataBase*	GetCurve(const JIndex index) const;

	JIndex	AddCurve(J2DPlotDataBase* data, const bool ownsData,
					 const JString& name);
	JIndex	AddCurve(J2DPlotDataBase* data, const bool ownsData,
					 const JString& name,
					 const bool line, const bool symbol);
	bool	AddCurve(const JArray<JFloat>& x, const JArray<JFloat>& y,
					 const bool listen, const JString& name,
					 JIndex* index,
					 const bool line = false, const bool symbol = true);
	void	RemoveCurve(const JIndex index);

	void	ResetScale();

	bool	IsZoomed() const;
	void	Zoom(const JRect& rect, const bool clean);

	void	GetXScale(JFloat* min, JFloat* max, JFloat* inc) const;
	void	SetXScale(const JFloat min, const JFloat max, const JFloat inc, const bool linear);
	void	GetYScale(JFloat* min, JFloat* max, JFloat* inc) const;
	void	SetYScale(const JFloat min, const JFloat max, const JFloat inc, const bool linear);

	bool	XAxisIsLinear() const;
	bool	YAxisIsLinear() const;

	bool	XErrorsAreVisible(const JIndex index);
	void	ShowXErrors(const JIndex index, const bool show);
	bool	YErrorsAreVisible(const JIndex index);
	void	ShowYErrors(const JIndex index, const bool show);

	bool	ArrayIsInCurve(JArray<JFloat>* testArray) const;
	void	RemoveCurvesContainingArray(JArray<JFloat>* testArray);

	JSize	GetSmoothSteps() const;
	void	GetDefaultXRange(JFloat* min, JFloat* max) const;
	void	SetDefaultXRange(const JFloat min, const JFloat max);
	void	GetDefaultYRange(JFloat* min, JFloat* max) const;
	void	SetDefaultYRange(const JFloat min, const JFloat max);

	bool	GetRange(JFloat* xmin, JFloat* xmax, JFloat* ymin, JFloat* ymax);
	void	CopyScaleToRange();
	void	SetRange(const JFloat xmin, const JFloat xmax,
					 const JFloat ymin, const JFloat ymax);
	void	ClearRange();
	bool	IsUsingRange() const;

	bool	GetCurveIndex(J2DPlotDataBase* data, JIndex* index);

	// Cursors

	bool	HasMarks() const;
	JSize	GetXMarkCount() const;
	JSize	GetYMarkCount() const;
	JFloat	GetXMarkValue(const JIndex index) const;
	JFloat	GetYMarkValue(const JIndex index) const;
	void	MarkCurrentCursor();
	void	MarkAllVisibleCursors();
	void	RemoveAllMarks();
	void	RemoveXMark(const JIndex index);
	void	RemoveYMark(const JIndex index);

	bool	CursorIsSelected() const;
	bool	XCursorVisible() const;
	void	ToggleXCursor();
	bool	YCursorVisible() const;
	void	ToggleYCursor();
	bool	DualCursorsVisible() const;
	void	ToggleDualCursors();

	bool	GetXCursorVal1(JFloat* value);
	bool	GetXCursorVal2(JFloat* value);
	bool	GetYCursorVal1(JFloat* value);
	bool	GetYCursorVal2(JFloat* value);

	// utility functions

	static void		FindRange(const JFloat min, const JFloat max,
							  JFloat* start, JFloat* end, JFloat* inc);

	static void		FindLogRange(const JFloat min, const JFloat max,
								 JFloat* start, JFloat* end, JFloat* inc);
	static JFloat	GetLogIncrement(const JFloat min, const JFloat max);

	static JString	FloatToString(const JFloat value);

protected:

	virtual void	PWDraw(JPainter& p, const JRect& rect);
	void			PWHandleMouseDown(const JPoint& pt, const JSize clickCount);
	void			PWHandleMouseDrag(const JPoint& pt);
	void			PWHandleMouseUp(const JPoint& pt, const bool cleanZoom);
	MouseCursor		GetMouseCursor(const JPoint& pt) const;
	bool			GetLegendIndex(const JPoint& pt, JIndex* curveIndex) const;

	// for cursors
	void	PWHandleKeyPress(const JUtf8Character& key, const bool reverse,
							 const bool interval, const bool skip);

	virtual JPainter*	PWCreateDragInsidePainter() = 0;
	virtual bool		PWGetDragPainter(JPainter** p) const = 0;
	virtual void		PWDeleteDragPainter() = 0;

	void			PWBoundsChanged();
	void			PWRefresh();
	virtual void	PWRefreshRect(const JRect& rect) = 0;
	virtual void	PWRedraw() = 0;
	virtual void	PWRedrawRect(const JRect& rect) = 0;
	virtual void	PWForceRefresh() = 0;

	JSize			PWGetWidth() const;
	JSize			PWGetHeight() const;
	virtual JSize	PWGetGUIWidth() const = 0;
	virtual JSize	PWGetGUIHeight() const = 0;
	virtual void	PWDisplayCursor(const MouseCursor cursor) = 0;

	virtual void	ProtectionChanged() = 0;

	virtual void	ChangeCurveOptions(const JIndex index) = 0;
	virtual void	ChangeLabels(const LabelSelection selection) = 0;
	virtual void	ChangeScale(const bool editXAxis) = 0;

	virtual JCoordinate	GetMarksHeight() const = 0;
	virtual bool		PrintMarks(JPagePrinter& p, const bool putOnSamePage,
									const JRect& partialPageRect) = 0;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	GetFrameGeometry(JCoordinate* xmin, JCoordinate* xmax,
							 JCoordinate* ymin, JCoordinate* ymax) const;
	JRect	GetFrameGeometry() const;

	JFloat	GetRealX(const JCoordinate frameX) const;
	JFloat	GetRealY(const JCoordinate frameY) const;
	void	GetRealPoint(const JPoint& framePoint, JFloat* x, JFloat* y) const;

	bool	GetFrameX(const JFloat x, JCoordinate* frameX) const;
	bool	GetFrameX(const JFloat x, JFloat* frameX) const;
	bool	GetFrameY(const JFloat y, JCoordinate* frameY) const;
	bool	GetFrameY(const JFloat y, JFloat* frameY) const;
	bool	GetFramePoint(const JFloat x, const JFloat y, JPoint* framePoint) const;

	const JArray<J2DCurveInfo>&	GetCurveInfoArray();
	void						SetCurveInfoArray(const JArray<J2DCurveInfo>& infoArray);

	virtual JString	BuildTickLabel(const bool isXAxis, const JFloat value,
								   const bool axisIsLinear,
								   const JInteger precision,
								   const bool forceExponent,
								   const JInteger exponent);
	JString	GetLogAxisString(const JFloat value) const;

private:

	JString	itsTitle;
	JString	itsXLabel;
	JString	itsYLabel;

	bool	itsShowLegendFlag;
	bool	itsShowGridFlag;
	bool	itsShowFrameFlag;

	bool	itsGeometryNeedsAdjustmentFlag;
	bool	itsAutomaticRefreshFlag;
	bool	itsIgnoreCurveChangedFlag;	// true => ignore CurveChanged messages

	JPtrArray<J2DPlotDataBase>*	itsCurves;
	JArray<J2DCurveInfo>*		itsCurveInfo;
	JArray<JColorID>*			itsColors;
	JArray<JSize>*				itsColorUsage;

	JFloat		itsXScale[4];
	JFloat		itsYScale[4];

	JSize		itsSymbolUsage [ kSymbolCount ];

	JString		itsFontName;
	JSize		itsFontSize;

	JCoordinate	itsXAxisStart;		// left
	JCoordinate	itsXAxisEnd;		// right
	JCoordinate	itsYAxisStart;		// bottom
	JCoordinate	itsYAxisEnd;		// top

	JFloat		itsXTrans;
	JFloat		itsYTrans;

	JSize		itsXDecimalPoints;
	JSize		itsYDecimalPoints;
	JInteger	itsXExp;
	JInteger	itsYExp;
	bool		itsForceXExp;
	bool		itsForceYExp;

	bool		itsXAxisIsLinear;
	bool		itsYAxisIsLinear;
	bool		itsIsZoomedFlag;
	bool		itsUseRealXStart;
	bool		itsUseRealYStart;

	bool		itsShowXMinorTics;
	bool		itsShowYMinorTics;

	const JColorID	itsBlackColor;
	const JColorID	itsWhiteColor;
	const JColorID	itsGrayColor;
	const JColorID	itsSelectionColor;

	bool	itsUsingRange;
	JFloat	itsRangeXMin;
	JFloat	itsRangeXMax;
	JFloat	itsRangeYMin;
	JFloat	itsRangeYMax;

	JSize	itsLineHeight;
	JSize	itsMaxCurveNameWidth;
	JSize	itsMaxXLabelWidth;
	JSize	itsLegendWidth;
	JRect	itsLegendRect;		// undefined unless itsShowLegendFlag
	JRect	itsTitleRect;
	JRect	itsXLabelRect;
	JRect	itsYLabelRect;
	JRect	itsXAxisRect;
	JRect	itsYAxisRect;

	// dragging

	JPoint	itsStartPt;
	JPoint	itsPrevPt;

	// Printing

	bool		itsIsPrintingFlag;
	JCoordinate	itsPrintWidth;
	JCoordinate	itsPrintHeight;

	// Cursors

	bool			itsXCursorVisible;
	bool			itsYCursorVisible;
	bool			itsDualCursors;
	JCoordinate		itsXCursorPos1;
	JCoordinate		itsXCursorPos2;
	JCoordinate		itsYCursorPos1;
	JCoordinate		itsYCursorPos2;
	JArray<JFloat>*	itsXMarks;
	JArray<JFloat>*	itsYMarks;
	bool			itsIsCursorDragging;
	JFloat			itsXCursorVal1;
	JFloat			itsXCursorVal2;
	JFloat			itsYCursorVal1;
	JFloat			itsYCursorVal2;
	CursorIndex		itsSelectedCursor;
	bool			itsXCursor1InitFlag;
	bool			itsXCursor2InitFlag;
	bool			itsYCursor1InitFlag;
	bool			itsYCursor2InitFlag;

private:

	void	HandleCurveChanged();
	void	AdjustGeometry(JPainter& p);
	void	UpdatePlot(const bool geometry = false,
						const bool scale = false);
	void	UpdateScale();
	void	UpdateScale(const bool allowResetToLinear, bool* linear,
						const JFloat min, const JFloat max,
						const bool clean, JFloat* scale);
	void	ResetScale(const bool allowResetToLinear);
	void	GetXDataRange(JFloat* min, JFloat* max) const;
	void	GetYDataRange(JFloat* min, JFloat* max) const;

	void	DrawAxes(JPainter& p);

	void	DrawTicks(JPainter& p);
	void	DrawXLogMinorTicks(JPainter& p, const JCoordinate yTick, const JSize tickCount, const JCoordinate yLabel);
	void	DrawYLogMinorTicks(JPainter& p, const JCoordinate xTick, const JSize tickCount, const JCoordinate xLabel, const bool xLabelEndKnown);
	void	DrawXTicks(JPainter& p, const JCoordinate yTick, const JCoordinate yLabel, const JSize tickCount);
	void	DrawXTick(JPainter& p, const JFloat value, const JCoordinate yVal,
						  const JCoordinate tickLength,
						  const JCoordinate yLabel,
						  const bool drawStr,
						  const bool lightGrid);
	void	DrawYTicks(JPainter& p, const JCoordinate xTick, const JCoordinate xLabel, const bool xLabelEndKnown, const JSize tickCount);
	void	DrawYTick(JPainter& p, const JFloat value, const JCoordinate xVal,
						  const JCoordinate tickLength,
						  const JCoordinate xLabel,
						  const JCoordinate xLabelEndKnown,
						  const bool drawStr,
						  const bool lightGrid);
	void	GetLogTicks(const JFloat min, const JFloat max, const JFloat inc,
						JArray<JFloat>* major,
						JArray<JFloat>* minor) const;

	void	DrawLabels(JPainter& p);
	void	DrawData(JPainter& p) const;
	void	DrawLegend(JPainter& p);

	void	DrawCurve(JPainter& p, const J2DPlotDataBase& curve,
					  const J2DCurveInfo& info, const J2DDataRect& visRect) const;
	void	DrawError(JPainter& p, const JPoint& pt,
						  const J2DPlotDataBase& curve, const J2DCurveInfo& info,
						  const JIndex index, const bool xLinear, const bool yLinear,
						  const J2DDataRect& visRect) const;

	void	DrawVector(JPainter& p, const J2DPlotDataBase& curve,
					   const J2DCurveInfo& info, const J2DDataRect& visRect) const;
	void	DrawVectorHead(JPainter& p,
						   const JPoint& headPt, const JPoint& tailPt,
						   const J2DVectorPoint& vector) const;

	void	DrawSymbol(JPainter& p, const JPoint& pt, const J2DSymbolType type) const;
	void	CalcVisibleRange(const JFloat scaleMin, const JFloat scaleMax,
							 const JFloat rangeMin, const JFloat rangeMax,
							 JFloat* min, JFloat* max) const;
	void	Interpolate(const JIndex index, const J2DPlotDataBase& data,
						const J2DDataRect& visRect,
						J2DDataPoint* data1, J2DDataPoint* data2,
						bool* move, bool* draw, bool* mark) const;
	void	Interpolate(const J2DDataRect& visRect,
						J2DDataPoint* data1, J2DDataPoint* data2,
						bool* move, bool* draw, bool* mark) const;
	bool	CalledByInterpolate(J2DDataPoint* data, const J2DDataRect& visRect,
								const JFloat slope,
								const bool horiz, const bool vert) const;
	bool	ConvertLog10(J2DDataPoint* data) const;
	JFloat	GetGraphValue(const JFloat value, const bool linear) const;

	void			SetClipping(JPainter& p) const;

	J2DSymbolType	GetFirstAvailableSymbol();
	JIndex			GetFirstAvailableColor();

	void			GetScale(const JFloat* scale, const bool linear,
							 JFloat* min, JFloat* max, JFloat* inc) const;
	void			SetScale(const JFloat min, const JFloat max, const JFloat inc,
							 const bool linear, JFloat* scale);

	void			SetPlotDecPlaces();
	static void		CalledBySetPlotDecPlaces(const JFloat* scale, const bool useExactRange,
											 const bool forceExp, const JCoordinate exp,
											 JSize* dpCount);
	static JSize	GetDecPlaces(const JFloat value);

	bool			GetMinimumPositiveValue(const bool isX, JFloat *min) const;

	static void		SetExp(const JFloat* scale, JInteger* exp, bool* force);
	static void		GetExp(const JFloat value, JInteger* exp);

	void			TruncateCurveName(JPainter& p, const JIndex index);

	// Cursors

	void			AdjustCursors();
	void			UpdateCursorInternals();
	void			DrawCursors(JPainter& p);
	void			DrawCursorLabels(JPainter& p);
	void			DrawMarks(JPainter& p);
	JSize			GetCursorLabelHeight(JPainter& p) const;
	CursorIndex		GetCursorIndex(const JPoint& pt) const;
	bool			MoveCursor(const JCoordinate min, const JCoordinate max,
								const JCoordinate delta, JCoordinate* cursorPos) const;

	// not allowed

	J2DPlotWidget(const J2DPlotWidget&) = delete;
	J2DPlotWidget& operator=(const J2DPlotWidget&) = delete;

public:

	static const JUtf8Byte* kTitleChanged;
	static const JUtf8Byte* kScaleChanged;
	static const JUtf8Byte* kGeometryChanged;
	static const JUtf8Byte* kPlotChanged;
	static const JUtf8Byte* kIsEmpty;
	static const JUtf8Byte* kCurveAdded;
	static const JUtf8Byte* kCurveRemoved;

	// Cursors
	static const JUtf8Byte* kCursorsChanged;
	static const JUtf8Byte* kCursorMarked;
	static const JUtf8Byte* kMarkRemoved;

	class TitleChanged : public JBroadcaster::Message
	{
	public:

		TitleChanged()
			:
			JBroadcaster::Message(kTitleChanged)
		{ };
	};

	class ScaleChanged : public JBroadcaster::Message
	{
	public:

		ScaleChanged()
			:
			JBroadcaster::Message(kScaleChanged)
		{ };
	};

	class GeometryChanged : public JBroadcaster::Message
	{
	public:

		GeometryChanged()
			:
			JBroadcaster::Message(kGeometryChanged)
		{ };
	};

	class PlotChanged : public JBroadcaster::Message
	{
	public:

		PlotChanged()
			:
			JBroadcaster::Message(kPlotChanged)
		{ };
	};


	class IsEmpty : public JBroadcaster::Message
	{
	public:

		IsEmpty()
			:
			JBroadcaster::Message(kIsEmpty)
		{ };
	};

	class CurveAdded : public JBroadcaster::Message
	{
	public:

		CurveAdded(const JIndex index)
			:
			JBroadcaster::Message(kCurveAdded),
			itsIndex(index)
		{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

	private:

		JIndex itsIndex;

	};

	class CurveRemoved : public JBroadcaster::Message
	{
	public:

		CurveRemoved(const JIndex index)
			:
			JBroadcaster::Message(kCurveRemoved),
			itsIndex(index)
		{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

	private:

		JIndex itsIndex;

	};

	class CursorsChanged : public JBroadcaster::Message
	{
	public:

		CursorsChanged
			(
			const bool showX,
			const bool showY,
			const bool dual,
			const JFloat x1,
			const JFloat x2,
			const JFloat y1,
			const JFloat y2
			)
			:
			JBroadcaster::Message(kCursorsChanged),
			itsShowX(showX),
			itsShowY(showY),
			itsDual(dual),
			itsX1(x1),
			itsX2(x2),
			itsY1(y1),
			itsY2(y2)
		{ };

		bool
		ShowX() const
		{
			return itsShowX;
		};

		bool
		ShowY() const
		{
			return itsShowY;
		};

		bool
		Dual() const
		{
			return itsDual;
		};

		JFloat
		GetX1() const
		{
			return itsX1;
		};

		JFloat
		GetX2() const
		{
			return itsX2;
		};

		JFloat
		GetY1() const
		{
			return itsY1;
		};

		JFloat
		GetY2() const
		{
			return itsY2;
		};

	private:

		bool	itsShowX;
		bool	itsShowY;
		bool	itsDual;
		JFloat	itsX1;
		JFloat	itsX2;
		JFloat	itsY1;
		JFloat	itsY2;
	};

	class CursorMarked : public JBroadcaster::Message
	{
	public:

		CursorMarked(const bool isXCursor, const JFloat value)
			:
			JBroadcaster::Message(kCursorMarked),
			itsIsXCursor(isXCursor),
			itsValue(value)
		{ };

		bool
		IsXCursor() const
		{
			return itsIsXCursor;
		};

		JFloat
		GetValue() const
		{
			return itsValue;
		};

	private:

		bool	itsIsXCursor;
		JFloat	itsValue;
	};

	class MarkRemoved : public JBroadcaster::Message
	{
	public:

		MarkRemoved(const bool isXCursor, const JIndex index)
			:
			JBroadcaster::Message(kMarkRemoved),
			itsIsXCursor(isXCursor),
			itsIndex(index)
		{ };

		bool
		IsXCursor() const
		{
			return itsIsXCursor;
		};

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

	private:

		bool	itsIsXCursor;
		JIndex	itsIndex;
	};
};


/*******************************************************************************
 PWBoundsChanged

 ******************************************************************************/

inline void
J2DPlotWidget::PWBoundsChanged()
{
	UpdatePlot(true);
}

/*******************************************************************************
 PWGetWidth (protected)

 ******************************************************************************/

inline JSize
J2DPlotWidget::PWGetWidth()
	const
{
	return itsIsPrintingFlag ? itsPrintWidth : PWGetGUIWidth();
}

/*******************************************************************************
 PWGetHeight (protected)

 ******************************************************************************/

inline JSize
J2DPlotWidget::PWGetHeight()
	const
{
	return itsIsPrintingFlag ? itsPrintHeight : PWGetGUIHeight();
}

/*******************************************************************************
 PWRefresh

 ******************************************************************************/

inline void
J2DPlotWidget::PWRefresh()
{
	if (itsAutomaticRefreshFlag)
	{
		PWForceRefresh();
	}
}

/*******************************************************************************
 AutoRefresh

 ******************************************************************************/

inline bool
J2DPlotWidget::WillAutoRefresh()
	const
{
	return itsAutomaticRefreshFlag;
}

inline void
J2DPlotWidget::ShouldAutoRefresh
	(
	const bool refresh
	)
{
	itsAutomaticRefreshFlag = refresh;
	if (refresh)
	{
		PWRefresh();
	}
}

/*******************************************************************************
 Font name

 ******************************************************************************/

inline const JString&
J2DPlotWidget::GetFontName()
	const
{
	return itsFontName;
}

inline void
J2DPlotWidget::SetFontName
	(
	const JString& name
	)
{
	itsFontName = name;
	UpdatePlot(true);
}

/*******************************************************************************
 Font size

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetFontSize()
	const
{
	return itsFontSize;
}

inline void
J2DPlotWidget::SetFontSize
	(
	const JSize size
	)
{
	itsFontSize = size;
	UpdatePlot(true);
}

/******************************************************************************
 Title

 ******************************************************************************/

inline const JString&
J2DPlotWidget::GetTitle()
	const
{
	return itsTitle;
}

inline void
J2DPlotWidget::SetTitle
	(
	const JString& xLabel
	)
{
	itsTitle = xLabel;
	Broadcast(TitleChanged());
	UpdatePlot();
}

/******************************************************************************
 Labels

 ******************************************************************************/

inline const JString&
J2DPlotWidget::GetXLabel()
	const
{
	return itsXLabel;
}

inline void
J2DPlotWidget::SetXLabel
	(
	const JString& xLabel
	)
{
	itsXLabel = xLabel;
	UpdatePlot();
}

inline const JString&
J2DPlotWidget::GetYLabel()
	const
{
	return itsYLabel;
}

inline void
J2DPlotWidget::SetYLabel
	(
	const JString& yLabel
	)
{
	itsYLabel = yLabel;
	UpdatePlot();
}

/*******************************************************************************
 SetLabels

 ******************************************************************************/

inline void
J2DPlotWidget::SetLabels
	(
	const JString& title,
	const JString& xLabel,
	const JString& yLabel
	)
{
	itsTitle  = title;
	itsXLabel = xLabel;
	itsYLabel = yLabel;
	Broadcast(TitleChanged());
	UpdatePlot();
}

/******************************************************************************
 Axes linear/log

 ******************************************************************************/

inline bool
J2DPlotWidget::XAxisIsLinear()
	const
{
	return itsXAxisIsLinear;
}

inline bool
J2DPlotWidget::YAxisIsLinear()
	const
{
	return itsYAxisIsLinear;
}

/*******************************************************************************
 LegendIsVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::LegendIsVisible()
	const
{
	return itsShowLegendFlag;
}

/*******************************************************************************
 FrameIsVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::FrameIsVisible()
	const
{
	return itsShowFrameFlag;
}

/*******************************************************************************
 GridIsVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::GridIsVisible()
	const
{
	return itsShowGridFlag;
}

/******************************************************************************
 DualCursorsVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::DualCursorsVisible()
	const
{
	return itsDualCursors;
}

/*******************************************************************************
 GetCurveCount

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetCurveCount()
	const
{
	return itsCurves->GetItemCount();
}

/*******************************************************************************
 GetCurve

 ******************************************************************************/

inline J2DPlotDataBase*
J2DPlotWidget::GetCurve
	(
	const JIndex index
	)
{
	return itsCurves->GetItem(index);
}

inline const J2DPlotDataBase*
J2DPlotWidget::GetCurve
	(
	const JIndex index
	)
	const
{
	return itsCurves->GetItem(index);
}

/*******************************************************************************
 GetCurveName

 ******************************************************************************/

inline const JString&
J2DPlotWidget::GetCurveName
	(
	const JIndex index
	)
	const
{
	return *itsCurveInfo->GetItem(index).name;
}

/*******************************************************************************
 GetCurveIndex

 ******************************************************************************/

inline bool
J2DPlotWidget::GetCurveIndex
	(
	J2DPlotDataBase*	data,
	JIndex*				index
	)
{
	return itsCurves->Find(data, index);
}

/******************************************************************************
 GetCurveInfoArray (protected)

 ******************************************************************************/

inline const JArray<J2DCurveInfo>&
J2DPlotWidget::GetCurveInfoArray()
{
	return *itsCurveInfo;
}

/*******************************************************************************
 CurveIsVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::CurveIsVisible
	(
	const JIndex index
	)
	const
{
	return itsCurveInfo->GetItem(index).show;
}

/*******************************************************************************
 LinesAreVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::LinesAreVisible
	(
	const JIndex index
	)
	const
{
	return itsCurveInfo->GetItem(index).lines;
}

/*******************************************************************************
 SymbolsAreVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::SymbolsAreVisible
	(
	const JIndex index
	)
	const
{
	return itsCurveInfo->GetItem(index).symbols;
}

/******************************************************************************
 XErrorsAreVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::XErrorsAreVisible
	(
	const JIndex index
	)
{
	return itsCurveInfo->GetItem(index).xerrors;
}

/******************************************************************************
 YErrorsAreVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::YErrorsAreVisible
	(
	const JIndex index
	)
{
	return itsCurveInfo->GetItem(index).yerrors;
}

/*******************************************************************************
 CurveIsProtected

 ******************************************************************************/

inline bool
J2DPlotWidget::CurveIsProtected
	(
	const JIndex index
	)
	const
{
	return itsCurveInfo->GetItem(index).protect;
}

/*******************************************************************************
 GetCurveColor

 ******************************************************************************/

inline JColorID
J2DPlotWidget::GetCurveColor
	(
	const JIndex index
	)
	const
{
	return itsCurveInfo->GetItem(index).color;
}

/*******************************************************************************
 AddColor

 ******************************************************************************/

inline void
J2DPlotWidget::AddColor
	(
	const JColorID color
	)
{
	itsColors->AppendItem(color);
	itsColorUsage->AppendItem(0);
}

/*******************************************************************************
 GetSymbolType

 ******************************************************************************/

inline J2DSymbolType
J2DPlotWidget::GetSymbolType
	(
	const JIndex index
	)
	const
{
	return itsCurveInfo->GetItem(index).symbol;
}

/******************************************************************************
 IsZoomed

 ******************************************************************************/

inline bool
J2DPlotWidget::IsZoomed()
	const
{
	return itsIsZoomedFlag;
}

/*******************************************************************************
 Axes scales

	If the increment is set to a negative value, the actual increment is
	calculated to produce a nice scale.

 ******************************************************************************/

inline void
J2DPlotWidget::GetXScale
	(
	JFloat* min,
	JFloat* max,
	JFloat* inc
	)
	const
{
	GetScale(itsXScale, itsXAxisIsLinear, min, max, inc);
}

inline void
J2DPlotWidget::SetXScale
	(
	const JFloat	min,
	const JFloat	max,
	const JFloat	inc,
	const bool	linear
	)
{
	itsXAxisIsLinear = linear;
	itsUseRealXStart = false;
	SetScale(min, max, inc, linear, itsXScale);
}

inline void
J2DPlotWidget::GetYScale
	(
	JFloat* min,
	JFloat* max,
	JFloat* inc
	)
	const
{
	GetScale(itsYScale, itsYAxisIsLinear, min, max, inc);
}

inline void
J2DPlotWidget::SetYScale
	(
	const JFloat	min,
	const JFloat	max,
	const JFloat	inc,
	const bool	linear
	)
{
	itsYAxisIsLinear = linear;
	itsUseRealYStart = false;
	SetScale(min, max, inc, linear, itsYScale);
}

/******************************************************************************
 IsUsingRange

 ******************************************************************************/

inline bool
J2DPlotWidget::IsUsingRange()
	const
{
	return itsUsingRange;
}

/*******************************************************************************
 GetRange

 ******************************************************************************/

inline bool
J2DPlotWidget::GetRange
	(
	JFloat* xmin,
	JFloat* xmax,
	JFloat* ymin,
	JFloat* ymax
	)
{
	*xmin = itsRangeXMin;
	*xmax = itsRangeXMax;
	*ymin = itsRangeYMin;
	*ymax = itsRangeYMax;
	return itsUsingRange;
}

/*******************************************************************************
 SetRange

 ******************************************************************************/

inline void
J2DPlotWidget::SetRange
	(
	const JFloat xmin,
	const JFloat xmax,
	const JFloat ymin,
	const JFloat ymax
	)
{
	itsUsingRange = true;
	itsRangeXMax  = xmax;
	itsRangeXMin  = xmin;
	itsRangeYMax  = ymax;
	itsRangeYMin  = ymin;
	Broadcast(PlotChanged());
}

/*******************************************************************************
 ClearRange

 ******************************************************************************/

inline void
J2DPlotWidget::ClearRange()
{
	if (itsUsingRange)
	{
		itsUsingRange = false;
		Broadcast(PlotChanged());
		PWRefresh();
	}
}

/*******************************************************************************
 CopyScaleToRange

 ******************************************************************************/

inline void
J2DPlotWidget::CopyScaleToRange()
{
	SetRange(itsXScale[kMin], itsXScale[kMax],
			 itsYScale[kMin], itsYScale[kMax]);
}

/******************************************************************************
 GetFrameGeometry

 ******************************************************************************/

inline void
J2DPlotWidget::GetFrameGeometry
	(
	JCoordinate* xmin,
	JCoordinate* xmax,
	JCoordinate* ymin,
	JCoordinate* ymax
	)
	const
{
	*xmin = itsXAxisStart;
	*xmax = itsXAxisEnd;
	*ymin = itsYAxisStart;
	*ymax = itsYAxisEnd;
}

inline JRect
J2DPlotWidget::GetFrameGeometry()
	const
{
	return JRect(itsYAxisEnd,   itsXAxisStart,
				 itsYAxisStart, itsXAxisEnd);
}

/*******************************************************************************
 GetRealPoint

 ******************************************************************************/

inline void
J2DPlotWidget::GetRealPoint
	(
	const JPoint& framePoint,
	JFloat* x,
	JFloat* y
	)
	const
{
	*x = GetRealX(framePoint.x);
	*y = GetRealY(framePoint.y);
}

/*******************************************************************************
 GetFramePoint

 ******************************************************************************/

inline bool
J2DPlotWidget::GetFrameX
	(
	const JFloat	x,
	JCoordinate*	frameX
	)
	const
{
	JFloat xp;
	const bool ok = GetFrameX(x, &xp);
	*frameX = JRound(xp);
	return ok;
}

inline bool
J2DPlotWidget::GetFrameY
	(
	const JFloat	y,
	JCoordinate*	frameY
	)
	const
{
	JFloat yp;
	const bool ok = GetFrameY(y, &yp);
	*frameY = JRound(yp);
	return ok;
}

inline bool
J2DPlotWidget::GetFramePoint
	(
	const JFloat	x,
	const JFloat	y,
	JPoint*			framePoint
	)
	const
{
	return GetFrameX(x, &(framePoint->x)) &&
				GetFrameY(y, &(framePoint->y));
}

/******************************************************************************
 GetSmoothSteps

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetSmoothSteps()
	const
{
	return PWGetWidth();
}

/******************************************************************************
 CursorIsSelected

 ******************************************************************************/

inline bool
J2DPlotWidget::CursorIsSelected()
	const
{
	return itsSelectedCursor != kNoCursor;
}

/******************************************************************************
 XCursorVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::XCursorVisible()
	const
{
	return itsXCursorVisible;
}

/******************************************************************************
 YCursorVisible

 ******************************************************************************/

inline bool
J2DPlotWidget::YCursorVisible()
	const
{
	return itsYCursorVisible;
}

/******************************************************************************
 GetXCursorVal1

 ******************************************************************************/

inline bool
J2DPlotWidget::GetXCursorVal1
	(
	JFloat* value
	)
{
	if (itsXCursorVisible)
	{
		*value = itsXCursorVal1;
		return true;
	}
	return false;
}

/******************************************************************************
 GetXCursorVal2

 ******************************************************************************/

inline bool
J2DPlotWidget::GetXCursorVal2
	(
	JFloat* value
	)
{
	if (itsXCursorVisible && itsDualCursors)
	{
		*value = itsXCursorVal2;
		return true;
	}
	return false;
}

/******************************************************************************
 GetYCursorVal1

 ******************************************************************************/

inline bool
J2DPlotWidget::GetYCursorVal1
	(
	JFloat* value
	)
{
	if (itsYCursorVisible)
	{
		*value = itsYCursorVal1;
		return true;
	}
	return false;
}

/******************************************************************************
 GetYCursorVal1

 ******************************************************************************/

inline bool
J2DPlotWidget::GetYCursorVal2
	(
	JFloat* value
	)
{
	if (itsYCursorVisible && itsDualCursors)
	{
		*value = itsYCursorVal2;
		return true;
	}
	return false;
}

/******************************************************************************
 HasMarks

 ******************************************************************************/

inline bool
J2DPlotWidget::HasMarks()
	const
{
	return !itsXMarks->IsEmpty() || !itsYMarks->IsEmpty();
}

/******************************************************************************
 GetXMarkCount

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetXMarkCount()
	const
{
	return itsXMarks->GetItemCount();
}

/******************************************************************************
 GetYMarkCount

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetYMarkCount()
	const
{
	return itsYMarks->GetItemCount();
}

/*******************************************************************************
 FloatToString (static)

 ******************************************************************************/

inline JString
J2DPlotWidget::FloatToString
	(
	const JFloat value
	)
{
	return JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, kSigDigitCount);
}

/*********************************************************************************
 Ignore CurveChanged messages

 ********************************************************************************/

inline bool
J2DPlotWidget::WillIgnoreCurveChanged()
	const
{
	return itsIgnoreCurveChangedFlag;
}

inline void
J2DPlotWidget::ShouldIgnoreCurveChanged
	(
	const bool ignore
	)
{
	itsIgnoreCurveChangedFlag = ignore;
	HandleCurveChanged();			// something has probably changed
}

#endif
