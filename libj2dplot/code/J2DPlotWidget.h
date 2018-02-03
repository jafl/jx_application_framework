/*********************************************************************************
 J2DPlotWidget.h

	Copyright (C) 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_J2DPlotWidget
#define _H_J2DPlotWidget

#include <JBroadcaster.h>
#include <jColor.h>
#include <JRect.h>
#include <JPtrArray-JString.h>
#include <JFontStyle.h>
#include <J2DCurveInfo.h>

class JPainter;
class JPagePrinter;
class JEPSPrinter;
class JPlotDataBase;
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

	J2DPlotWidget(const JColorIndex black, const JColorIndex white,
				  const JColorIndex gray, const JColorIndex selection);

	virtual ~J2DPlotWidget();

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

	JBoolean		WillAutoRefresh() const;
	void			ShouldAutoRefresh(const JBoolean refresh);

	JBoolean		WillIgnoreCurveChanged() const;
	void			ShouldIgnoreCurveChanged(const JBoolean ignore);

	void			SetLabels(const JCharacter* title, const JCharacter* xLabel, const JCharacter* yLabel);
	const JString&	GetXLabel() const;
	void			SetXLabel(const JCharacter* xLabel);
	const JString&	GetYLabel() const;
	void			SetYLabel(const JCharacter* yLabel);
	const JString&	GetTitle() const;
	void			SetTitle(const JCharacter* xLabel);


	const JString&	GetFontName() const;
	void			SetFontName(const JCharacter* name);

	JSize			GetFontSize() const;
	void			SetFontSize(const JSize size);

	JBoolean		LegendIsVisible() const;
	void			ShowLegend(const JBoolean show = kJTrue);
	JBoolean		FrameIsVisible() const;
	void			ShowFrame(const JBoolean show = kJTrue);
	JBoolean		GridIsVisible() const;
	void			ShowGrid(const JBoolean show = kJTrue);

	JBoolean		CurveIsVisible(const JIndex curveIndex) const;
	void			ShowCurve(const JIndex curveIndex, const JBoolean show);
	void			ShowAllCurves();
	void			HideAllOtherCurves(const JIndex curveIndex);
	JBoolean		LinesAreVisible(const JIndex curveIndex) const;
	void			ShowLines(const JIndex curveIndex, const JBoolean show = kJTrue);
	JBoolean		SymbolsAreVisible(const JIndex curveIndex) const;
	void			ShowSymbols(const JIndex curveIndex, const JBoolean show = kJTrue);
	void			SetCurveStyle(const JIndex curveIndex, const JBoolean lines,
								  const JBoolean symbols);
	JBoolean		CurveIsProtected(const JIndex curveIndex) const;
	void			ProtectCurve(const JIndex curveIndex, const JBoolean protect = kJTrue);

	JSize			GetCurveCount() const;

	const JString&	GetCurveName(const JIndex index) const;
	void			SetCurveName(const JIndex index, const JCharacter* name);

	void			AddColor(const JColorIndex color);
	JColorIndex		GetCurveColor(const JIndex index) const;
	J2DSymbolType	GetSymbolType(const JIndex index) const;

	JPlotDataBase&			GetCurve(const JIndex index);
	const JPlotDataBase&	GetCurve(const JIndex index) const;

	JIndex			AddCurve(JPlotDataBase* data, const JBoolean ownsData,
							 const JCharacter* name);
	JIndex			AddCurve(JPlotDataBase* data, const JBoolean ownsData,
							 const JCharacter* name,
							 const JBoolean line, const JBoolean symbol);
	JBoolean		AddCurve(JArray<JFloat>& x, JArray<JFloat>& y,
							 const JBoolean listen, const JCharacter* name,
							 JIndex* index,
							 const JBoolean line = kJFalse, const JBoolean symbol = kJTrue);
	void			RemoveCurve(const JIndex index);

	void			ResetScale();

	JBoolean		IsZoomed() const;
	void			Zoom(const JRect rect, const JBoolean clean);

	void			GetXScale(JFloat* min, JFloat* max, JFloat* inc) const;
	void			SetXScale(const JFloat min, const JFloat max, const JFloat inc, const JBoolean linear);
	void			GetYScale(JFloat* min, JFloat* max, JFloat* inc) const;
	void			SetYScale(const JFloat min, const JFloat max, const JFloat inc, const JBoolean linear);

	JBoolean		XAxisIsLinear() const;
	JBoolean		YAxisIsLinear() const;

	JBoolean		XErrorsAreVisible(const JIndex index);
	void			ShowXErrors(const JIndex index, const JBoolean show);
	JBoolean		YErrorsAreVisible(const JIndex index);
	void			ShowYErrors(const JIndex index, const JBoolean show);

	JBoolean		ArrayIsInCurve(JArray<JFloat>* testArray);
	void			RemoveCurvesContainingArray(JArray<JFloat>* testArray);

	JSize			GetSmoothSteps() const;
	void			GetDefaultXRange(JFloat* min, JFloat* max) const;
	void			SetDefaultXRange(const JFloat min, const JFloat max);
	void			GetDefaultYRange(JFloat* min, JFloat* max) const;
	void			SetDefaultYRange(const JFloat min, const JFloat max);

	JBoolean		GetRange(JFloat* xmin, JFloat* xmax, JFloat* ymin, JFloat* ymax);
	void			CopyScaleToRange();
	void			SetRange(const JFloat xmin, const JFloat xmax,
							 const JFloat ymin, const JFloat ymax);
	void			ClearRange();
	JBoolean		IsUsingRange() const;

	JBoolean		GetCurveIndex(JPlotDataBase* data, JIndex* index);

	// Cursors

	JBoolean		HasMarks() const;
	JSize			GetXMarkCount() const;
	JSize			GetYMarkCount() const;
	JFloat			GetXMarkValue(const JIndex index) const;
	JFloat			GetYMarkValue(const JIndex index) const;
	void			MarkCurrentCursor();
	void			MarkAllVisibleCursors();
	void			RemoveAllMarks();
	void			RemoveXMark(const JIndex index);
	void			RemoveYMark(const JIndex index);

	JBoolean		CursorIsSelected();
	JBoolean		XCursorVisible();
	void			ToggleXCursor();
	JBoolean		YCursorVisible();
	void			ToggleYCursor();
	JBoolean		DualCursorsVisible();
	void			ToggleDualCursors();

	JBoolean		GetXCursorVal1(JFloat* value);
	JBoolean		GetXCursorVal2(JFloat* value);
	JBoolean		GetYCursorVal1(JFloat* value);
	JBoolean		GetYCursorVal2(JFloat* value);

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
	void			PWHandleMouseUp(const JPoint& pt, const JBoolean cleanZoom);
	MouseCursor		GetMouseCursor(const JPoint& pt) const;
	JBoolean		GetLegendIndex(const JPoint& pt, JIndex* curveIndex) const;

	// for cursors
	void	PWHandleKeyPress(const int key, const JBoolean reverse,
							 const JBoolean interval, const JBoolean skip);

	virtual JPainter*	PWCreateDragInsidePainter() = 0;
	virtual JBoolean	PWGetDragPainter(JPainter** p) const = 0;
	virtual void		PWDeleteDragPainter() = 0;

	void			PWBoundsChanged();
	void			PWRefresh();
	virtual void	PWRefreshRect(const JRect& rect) = 0;
	virtual void	PWRedraw()						= 0;
	virtual void	PWRedrawRect(const JRect& rect) = 0;
	virtual void	PWForceRefresh()				= 0;

	JSize			PWGetWidth() const;
	JSize			PWGetHeight() const;
	virtual JSize	PWGetGUIWidth() const			= 0;
	virtual JSize	PWGetGUIHeight() const			= 0;
	virtual void	PWDisplayCursor(const MouseCursor cursor) = 0;

	virtual void	ProtectionChanged()			= 0;

	virtual void	ChangeCurveOptions(const JIndex index)			= 0;
	virtual void	ChangeLabels(const LabelSelection selection)	= 0;
	virtual void	ChangeScale(const JBoolean xAxis)				= 0;

	virtual JCoordinate	GetMarksHeight() const = 0;
	virtual JBoolean	PrintMarks(JPagePrinter& p, const JBoolean putOnSamePage,
									const JRect& partialPageRect) = 0;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	void			GetFrameGeometry(JCoordinate* xmin, JCoordinate* xmax,
									 JCoordinate* ymin, JCoordinate* ymax) const;
	JRect			GetFrameGeometry() const;

	JFloat			GetRealX(const JCoordinate frameX);
	JFloat			GetRealY(const JCoordinate frameY);
	void			GetRealPoint(const JPoint& framePoint, JFloat* x, JFloat* y);

	JBoolean		GetFrameX(const JFloat x, JCoordinate* frameX) const;
	JBoolean		GetFrameX(const JFloat x, JFloat* frameX) const;
	JBoolean		GetFrameY(const JFloat y, JCoordinate* frameY) const;
	JBoolean		GetFrameY(const JFloat y, JFloat* frameY) const;
	JBoolean		GetFramePoint(const JFloat x, const JFloat y, JPoint* framePoint) const;

	const JArray<J2DCurveInfo>&	GetCurveInfoArray();
	void						SetCurveInfoArray(const JArray<J2DCurveInfo>& infoArray);

	virtual JString	BuildTickLabel(const JBoolean isXAxis, const JFloat value,
								   const JBoolean axisIsLinear,
								   const JInteger precision,
								   const JBoolean forceExponent,
								   const JInteger exponent);
	JString			GetLogAxisString(const JFloat value);

private:

	JString			itsTitle;
	JString			itsXLabel;
	JString			itsYLabel;

	JBoolean		itsShowLegendFlag;
	JBoolean		itsShowGridFlag;
	JBoolean		itsShowFrameFlag;

	JBoolean		itsGeometryNeedsAdjustmentFlag;
	JBoolean		itsAutomaticRefreshFlag;
	JBoolean		itsIgnoreCurveChangedFlag;	// kJTrue => ignore CurveChanged messages

	JPtrArray<JPlotDataBase>*	itsCurves;
	JArray<J2DCurveInfo>*		itsCurveInfo;
	JArray<JColorIndex>*		itsColors;
	JArray<JSize>*				itsColorUsage;

	JFloat			itsXScale[4];
	JFloat			itsYScale[4];

	JSize			itsSymbolUsage [ kSymbolCount ];

	JString			itsFontName;
	JSize			itsFontSize;

	JCoordinate		itsXAxisStart;		// left
	JCoordinate		itsXAxisEnd;		// right
	JCoordinate		itsYAxisStart;		// bottom
	JCoordinate		itsYAxisEnd;		// top

	JFloat			itsXTrans;
	JFloat			itsYTrans;

	JSize			itsXDecimalPoints;
	JSize			itsYDecimalPoints;
	JInteger		itsXExp;
	JInteger		itsYExp;
	JBoolean		itsForceXExp;
	JBoolean		itsForceYExp;

	JBoolean		itsXAxisIsLinear;
	JBoolean		itsYAxisIsLinear;
	JBoolean		itsIsZoomedFlag;
	JBoolean		itsUseRealXStart;
	JBoolean		itsUseRealYStart;

	JBoolean		itsShowXMinorTics;
	JBoolean		itsShowYMinorTics;

	const JColorIndex	itsBlackColor;
	const JColorIndex	itsWhiteColor;
	const JColorIndex	itsGrayColor;
	const JColorIndex	itsSelectionColor;

	JBoolean		itsUsingRange;
	JFloat			itsRangeXMax;
	JFloat			itsRangeXMin;
	JFloat			itsRangeYMax;
	JFloat			itsRangeYMin;

	JSize			itsLineHeight;
	JSize			itsMaxCurveNameWidth;
	JSize			itsMaxXLabelWidth;
	JSize			itsLegendWidth;
	JRect			itsLegendRect;		// undefined unless itsShowLegendFlag
	JRect			itsTitleRect;
	JRect			itsXLabelRect;
	JRect			itsYLabelRect;
	JRect			itsXAxisRect;
	JRect			itsYAxisRect;

	// dragging

	JPoint			itsStartPt;
	JPoint			itsPrevPt;

	// Printing

	JBoolean		itsIsPrintingFlag;
	JCoordinate		itsPrintWidth;
	JCoordinate		itsPrintHeight;

	// Cursors

	JBoolean		itsXCursorVisible;
	JBoolean		itsYCursorVisible;
	JBoolean		itsDualCursors;
	JCoordinate		itsXCursorPos1;
	JCoordinate		itsXCursorPos2;
	JCoordinate		itsYCursorPos1;
	JCoordinate		itsYCursorPos2;
	JArray<JFloat>*	itsXMarks;
	JArray<JFloat>*	itsYMarks;
	JBoolean		itsIsCursorDragging;
	JFloat			itsXCursorVal1;
	JFloat			itsXCursorVal2;
	JFloat			itsYCursorVal1;
	JFloat			itsYCursorVal2;
	CursorIndex		itsSelectedCursor;
	JBoolean		itsXCursor1InitFlag;
	JBoolean		itsXCursor2InitFlag;
	JBoolean		itsYCursor1InitFlag;
	JBoolean		itsYCursor2InitFlag;

private:

	void			HandleCurveChanged();
	void			AdjustGeometry(JPainter& p);
	void			UpdatePlot(const JBoolean geometry = kJFalse,
								const JBoolean scale = kJFalse);
	void			UpdateScale();
	void			UpdateScale(const JBoolean allowResetToLinear, JBoolean* linear,
								const JFloat min, const JFloat max,
								const JBoolean clean, JFloat* scale);
	void			ResetScale(const JBoolean allowResetToLinear);
	void			GetXDataRange(JFloat* min, JFloat* max) const;
	void			GetYDataRange(JFloat* min, JFloat* max) const;

	void			DrawAxes(JPainter& p);

	void			DrawTicks(JPainter& p);
	void			DrawXLogMinorTicks(JPainter& p, const JCoordinate yTick, const JSize tickCount, const JCoordinate yLabel);
	void			DrawYLogMinorTicks(JPainter& p, const JCoordinate xTick, const JSize tickCount, const JCoordinate xLabel, const JBoolean xLabelEndKnown);
	void			DrawXTicks(JPainter& p, const JCoordinate yTick, const JCoordinate yLabel, const JSize tickCount);
	void			DrawXTick(JPainter& p, const JFloat value, const JCoordinate yVal,
							  const JCoordinate tickLength,
							  const JCoordinate yLabel,
							  const JBoolean drawStr,
							  const JBoolean lightGrid);
	void			DrawYTicks(JPainter& p, const JCoordinate xTick, const JCoordinate xLabel, const JBoolean xLabelEndKnown, const JSize tickCount);
	void			DrawYTick(JPainter& p, const JFloat value, const JCoordinate xVal,
							  const JCoordinate tickLength,
							  const JCoordinate xLabel,
							  const JCoordinate xLabelEndKnown,
							  const JBoolean drawStr,
							  const JBoolean lightGrid);
	void			GetLogTicks(const JFloat min, const JFloat max, const JFloat inc,
								JArray<JFloat>* major,
								JArray<JFloat>* minor);

	void			DrawLabels(JPainter& p);
	void			DrawData(JPainter& p) const;
	void			DrawLegend(JPainter& p);

	void			DrawCurve(JPainter& p, const JPlotDataBase& curve,
							  const J2DCurveInfo& info, const J2DDataRect& visRect) const;
	void			DrawError(JPainter& p, const JPoint& pt,
							  const JPlotDataBase& curve, const J2DCurveInfo& info,
							  const JIndex index, const JBoolean xLinear, const JBoolean yLinear,
							  const J2DDataRect& visRect) const;

	void			DrawVector(JPainter& p, const JPlotDataBase& curve,
							   const J2DCurveInfo& info, const J2DDataRect& visRect) const;
	void			DrawVectorHead(JPainter& p,
								   const JPoint& headPt, const JPoint& tailPt,
								   const J2DVectorPoint& vector) const;

	void			DrawSymbol(JPainter& p, const JPoint& pt, const J2DSymbolType type) const;
	void			CalcVisibleRange(const JFloat scaleMin, const JFloat scaleMax,
									 const JFloat rangeMin, const JFloat rangeMax,
									 JFloat* min, JFloat* max) const;
	void			Interpolate(const JIndex index, const JPlotDataBase& data,
								const J2DDataRect& visRect,
								J2DDataPoint* data1, J2DDataPoint* data2,
								JBoolean* move, JBoolean* draw, JBoolean* mark) const;
	void			Interpolate(const J2DDataRect& visRect,
								J2DDataPoint* data1, J2DDataPoint* data2,
								JBoolean* move, JBoolean* draw, JBoolean* mark) const;
	JBoolean		Interpolate1(J2DDataPoint* data, const J2DDataRect& visRect,
								 const JFloat slope,
								 const JBoolean horiz, const JBoolean vert) const;
	JBoolean		ConvertLog10(J2DDataPoint* data) const;
	JFloat			GetGraphValue(const JFloat value, const JBoolean linear) const;

	void			SetClipping(JPainter& p) const;

	J2DSymbolType	GetFirstAvailableSymbol();
	JIndex			GetFirstAvailableColor();

	void			GetScale(const JFloat* scale, const JBoolean linear,
							 JFloat* min, JFloat* max, JFloat* inc) const;
	void			SetScale(const JFloat min, const JFloat max, const JFloat inc,
							 const JBoolean linear, JFloat* scale);

	void			SetPlotDecPlaces();
	static void		SetPlotDecPlaces1(const JFloat* scale, const JBoolean useExactRange,
									  const JBoolean forceExp, const JCoordinate exp,
									  JSize* dpCount);
	static JSize	GetDecPlaces(const JFloat value);

	JBoolean		GetMinimumPositiveValue(const JBoolean isX, JFloat *min);

	static void		SetExp(const JFloat* scale, JInteger* exp, JBoolean* force);
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
	JBoolean		MoveCursor(const JCoordinate min, const JCoordinate max,
								const JCoordinate delta, JCoordinate* cursorPos) const;

	// not allowed

	J2DPlotWidget(const J2DPlotWidget& source);
	const J2DPlotWidget& operator=(const J2DPlotWidget& source);

public:

	static const JCharacter* kTitleChanged;
	static const JCharacter* kScaleChanged;
	static const JCharacter* kGeometryChanged;
	static const JCharacter* kPlotChanged;
	static const JCharacter* kIsEmpty;
	static const JCharacter* kCurveAdded;
	static const JCharacter* kCurveRemoved;

	// Cursors
	static const JCharacter* kCursorsChanged;
	static const JCharacter* kCursorMarked;
	static const JCharacter* kMarkRemoved;

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
			const JBoolean showX,
			const JBoolean showY,
			const JBoolean dual,
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

			JBoolean
			ShowX() const
			{
			return itsShowX;
			};

			JBoolean
			ShowY() const
			{
			return itsShowY;
			};

			JBoolean
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

			JBoolean	itsShowX;
			JBoolean	itsShowY;
			JBoolean	itsDual;
			JFloat		itsX1;
			JFloat		itsX2;
			JFloat		itsY1;
			JFloat		itsY2;
	};

	class CursorMarked : public JBroadcaster::Message
	{
	public:

		CursorMarked(const JBoolean isXCursor, const JFloat value)
			:
			JBroadcaster::Message(kCursorMarked),
			itsIsXCursor(isXCursor),
			itsValue(value)
			{ };

		JBoolean
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

		JBoolean	itsIsXCursor;
		JFloat		itsValue;
	};

	class MarkRemoved : public JBroadcaster::Message
	{
	public:

		MarkRemoved(const JBoolean isXCursor, const JIndex index)
			:
			JBroadcaster::Message(kMarkRemoved),
			itsIsXCursor(isXCursor),
			itsIndex(index)
			{ };

		JBoolean
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

		JBoolean	itsIsXCursor;
		JIndex		itsIndex;
	};
};


/*******************************************************************************
 PWBoundsChanged

 ******************************************************************************/

inline void
J2DPlotWidget::PWBoundsChanged()
{
	UpdatePlot(kJTrue);
}

/*******************************************************************************
 PWGetWidth (protected)

 ******************************************************************************/

inline JSize
J2DPlotWidget::PWGetWidth()
	const
{
	return (itsIsPrintingFlag ? itsPrintWidth : PWGetGUIWidth());
}

/*******************************************************************************
 PWGetHeight (protected)

 ******************************************************************************/

inline JSize
J2DPlotWidget::PWGetHeight()
	const
{
	return (itsIsPrintingFlag ? itsPrintHeight : PWGetGUIHeight());
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

inline JBoolean
J2DPlotWidget::WillAutoRefresh()
	const
{
	return itsAutomaticRefreshFlag;
}

inline void
J2DPlotWidget::ShouldAutoRefresh
	(
	const JBoolean refresh
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
	const JCharacter* name
	)
{
	itsFontName = name;
	UpdatePlot(kJTrue);
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
	UpdatePlot(kJTrue);
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
	const JCharacter* xLabel
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
	const JCharacter* xLabel
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
	const JCharacter* yLabel
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
	const JCharacter* title,
	const JCharacter* xLabel,
	const JCharacter* yLabel
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

inline JBoolean
J2DPlotWidget::XAxisIsLinear()
	const
{
	return itsXAxisIsLinear;
}

inline JBoolean
J2DPlotWidget::YAxisIsLinear()
	const
{
	return itsYAxisIsLinear;
}

/*******************************************************************************
 LegendIsVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::LegendIsVisible()
	const
{
	return itsShowLegendFlag;
}

/*******************************************************************************
 FrameIsVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::FrameIsVisible()
	const
{
	return itsShowFrameFlag;
}

/*******************************************************************************
 GridIsVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::GridIsVisible()
	const
{
	return itsShowGridFlag;
}

/*******************************************************************************
 GetCurveCount

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetCurveCount()
	const
{
	return itsCurves->GetElementCount();
}

/*******************************************************************************
 GetCurve

 ******************************************************************************/

inline JPlotDataBase&
J2DPlotWidget::GetCurve
	(
	const JIndex index
	)
{
	return *(itsCurves->GetElement(index));
}

inline const JPlotDataBase&
J2DPlotWidget::GetCurve
	(
	const JIndex index
	)
	const
{
	return *(itsCurves->GetElement(index));
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
	return *((itsCurveInfo->GetElement(index)).name);
}

/*******************************************************************************
 GetCurveIndex

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::GetCurveIndex
	(
	JPlotDataBase*	data,
	JIndex*			index
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

inline JBoolean
J2DPlotWidget::CurveIsVisible
	(
	const JIndex index
	)
	const
{
	return (itsCurveInfo->GetElement(index)).show;
}

/*******************************************************************************
 LinesAreVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::LinesAreVisible
	(
	const JIndex index
	)
	const
{
	return (itsCurveInfo->GetElement(index)).lines;
}

/*******************************************************************************
 SymbolsAreVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::SymbolsAreVisible
	(
	const JIndex index
	)
	const
{
	return (itsCurveInfo->GetElement(index)).symbols;
}

/******************************************************************************
 XErrorsAreVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::XErrorsAreVisible
	(
	const JIndex index
	)
{
	return (itsCurveInfo->GetElement(index)).xerrors;
}

/******************************************************************************
 YErrorsAreVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::YErrorsAreVisible
	(
	const JIndex index
	)
{
	return (itsCurveInfo->GetElement(index)).yerrors;
}

/*******************************************************************************
 CurveIsProtected

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::CurveIsProtected
	(
	const JIndex index
	)
	const
{
	return (itsCurveInfo->GetElement(index)).protect;
}

/*******************************************************************************
 GetCurveColor

 ******************************************************************************/

inline JColorIndex
J2DPlotWidget::GetCurveColor
	(
	const JIndex index
	)
	const
{
	return (itsCurveInfo->GetElement(index)).color;
}

/*******************************************************************************
 AddColor

 ******************************************************************************/

inline void
J2DPlotWidget::AddColor
	(
	const JColorIndex color
	)
{
	itsColors->AppendElement(color);
	itsColorUsage->AppendElement(0);
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
	return (itsCurveInfo->GetElement(index)).symbol;
}

/******************************************************************************
 IsZoomed

 ******************************************************************************/

inline JBoolean
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
	const JBoolean	linear
	)
{
	itsXAxisIsLinear = linear;
	itsUseRealXStart = kJFalse;
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
	const JBoolean	linear
	)
{
	itsYAxisIsLinear = linear;
	itsUseRealYStart = kJFalse;
	SetScale(min, max, inc, linear, itsYScale);
}

/******************************************************************************
 IsUsingRange

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::IsUsingRange()
	const
{
	return itsUsingRange;
}

/*******************************************************************************
 GetRange

 ******************************************************************************/

inline JBoolean
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
	itsUsingRange = kJTrue;
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
		itsUsingRange = kJFalse;
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
{
	*x = GetRealX(framePoint.x);
	*y = GetRealY(framePoint.y);
}

/*******************************************************************************
 GetFramePoint

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::GetFrameX
	(
	const JFloat	x,
	JCoordinate*	frameX
	)
	const
{
	JFloat xp;
	const JBoolean ok = GetFrameX(x, &xp);
	*frameX = JRound(xp);
	return ok;
}

inline JBoolean
J2DPlotWidget::GetFrameY
	(
	const JFloat	y,
	JCoordinate*	frameY
	)
	const
{
	JFloat yp;
	const JBoolean ok = GetFrameY(y, &yp);
	*frameY = JRound(yp);
	return ok;
}

inline JBoolean
J2DPlotWidget::GetFramePoint
	(
	const JFloat	x,
	const JFloat	y,
	JPoint*			framePoint
	)
	const
{
	return JI2B(GetFrameX(x, &(framePoint->x)) &&
				GetFrameY(y, &(framePoint->y)));
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

inline JBoolean
J2DPlotWidget::CursorIsSelected()
{
	return JConvertToBoolean(itsSelectedCursor != kNoCursor);
}

/******************************************************************************
 XCursorVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::XCursorVisible()
{
	return itsXCursorVisible;
}

/******************************************************************************
 YCursorVisible

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::YCursorVisible()
{
	return itsYCursorVisible;
}

/******************************************************************************
 GetXCursorVal1

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::GetXCursorVal1
	(
	JFloat* value
	)
{
	if (itsXCursorVisible)
		{
		*value = itsXCursorVal1;
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 GetXCursorVal2

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::GetXCursorVal2
	(
	JFloat* value
	)
{
	if (itsXCursorVisible && itsDualCursors)
		{
		*value = itsXCursorVal2;
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 GetYCursorVal1

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::GetYCursorVal1
	(
	JFloat* value
	)
{
	if (itsYCursorVisible)
		{
		*value = itsYCursorVal1;
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 GetYCursorVal1

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::GetYCursorVal2
	(
	JFloat* value
	)
{
	if (itsYCursorVisible && itsDualCursors)
		{
		*value = itsYCursorVal2;
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 HasMarks

 ******************************************************************************/

inline JBoolean
J2DPlotWidget::HasMarks()
	const
{
	return JNegate( itsXMarks->IsEmpty() && itsYMarks->IsEmpty() );
}

/******************************************************************************
 GetXMarkCount

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetXMarkCount()
	const
{
	return itsXMarks->GetElementCount();
}

/******************************************************************************
 GetYMarkCount

 ******************************************************************************/

inline JSize
J2DPlotWidget::GetYMarkCount()
	const
{
	return itsYMarks->GetElementCount();
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

inline JBoolean
J2DPlotWidget::WillIgnoreCurveChanged()
	const
{
	return itsIgnoreCurveChangedFlag;
}

inline void
J2DPlotWidget::ShouldIgnoreCurveChanged
	(
	const JBoolean ignore
	)
{
	itsIgnoreCurveChangedFlag = ignore;
	HandleCurveChanged();			// something has probably changed
}

#endif
