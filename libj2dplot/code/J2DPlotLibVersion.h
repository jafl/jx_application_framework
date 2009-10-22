/******************************************************************************
 J2DPlotLibVersion.h

	Defines current version of J2DPlot library

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J2DPlotLibVersion
#define _H_J2DPlotLibVersion

// These have to be #defined so they can be used for conditional compilation.

#define CURRENT_J2DPLOT_MAJOR_VERSION	1
#define CURRENT_J2DPLOT_MINOR_VERSION	0
#define CURRENT_J2DPLOT_PATCH_VERSION	0

// This is mainly provided so programmers can see the official version number.

static const char* kCurrentJ2DPlotLibVersionStr = "1.0.0";

// version 1.1.0:
// J2DPlotWidget:
//	Grid lines are gray, except for the axes.
//	Cursor positions are set only after first being shown.  This ensures that
//		they will be visible the first time.
//	When a curve is removed, the scale is only reset if the plot is not zoomed.
// JX2DPlotWidget:
//	Menu handlers only check for widget focus if the menus are shared between
//		multiple widgets.
// JX2DCursorMarkTableDir:
//	Removed Close button.
// J2DPlotData:
//	It is now safe to delete the data arrays, as long as you don't try to access
//		the plot data afterwards.
// J2DVectorData:
//	It is now safe to delete the data arrays, as long as you don't try to access
//		the plot data afterwards.

// version 1.0.0:
// J2DPlotWidget:
//	Added virtual function BuildTickLabel() to allow derived classes to
//		draw tick labels in a custom way.
//	Fixed bugs in tick label decimal places computations.

// version 0.7.1:
//	JX2DPlotPrintEPSDialog:
//		Fixed bug so unit is now displayed as the menu title.

// version 0.7.0:
//	Converted all dialogs to use j2dplot.fd
//	Renamed classes to use J2D and JX2D prefixes.
//	Eliminated fitplot.h.  FindRange is now a static member of J2DPlotWidget.
//		Rearranged arguments to FindRange() for consistency.
//	JPlotDataBase:
//		Renamed PlotDataType to Type.
//		Defined virtual IsFunction().
//		*** Added BroadcastCurveChanged().  All derived classes must use this
//			instead of broadcasting directly.  This allows the message to be
//			shut off when speed is required.
//		Added ShouldBroadcastChanged() so messages can be turned off temporarily.
//		Changed prototypes of Get*Range() functions to include whether each axis is linear.
//	JPlotFunctionBase:
//		IsFunction() returns kJTrue.
//		Fixed GetYRange() so it notices when the x scale is logarithmic.
//	J2DPlotJFunction:
//		Added GetVariableList() and SetFunction().
//	J2DPlotData:
//		Fixed memory leaks.
//		Removed GetX/YMin/Max().
//		Optimized GetX/YRange().
//		GetX/YData() now returns a reference to the array instead of a pointer.
//		Renamed GetX/YErrorData() to GetX/YPErrorData().
//		Fixed prototype of ArrayInData() to take "const JArray<JFloat>*".
//	J2DVectorData:
//		Added default constructor and functions to add/remove data points.
//		No longer copies data when constructed to listen to set of JArray's.
//		Fixed Get*Data() so they return references.
//		Added IsValid() and IsListening().
//	J2DCurveInfo:
//		Converted points member to a function.
//	J2DPlotWidget:
//		Added version of Print() for EPS.
//		Added DrawForPrint().
//		Added hiddenCurveLabel argument to constructor.
//		Added static GetPSPortrait/LandscapePrintSize().
//		Added HasMarks().
//		Removed GetCurveNames() because it was unsafe.
//		Rearranged arguments to FindLogRange() and GetLogIncrement() for consistency.
//		Promoted FindLogRange() and GetLogIncrement() to static public.
//		Fixed PostScript printing so plot+marks are always centered vertically.
//		Removed points argument from AddCurve().
//		Fixed version of AddCurve() that takes raw data so it returns JBoolean.
//		Split version of AddCurve() that takes JPlotDataBase into two versions.
//			Simple version automatically selects lines for functions and symbols for data.
//		Added ShowAllCurves() and HideAllOtherCurves().
//		Added GetLegendIndex().
//		Renamed ShowSymbol() to ShowSymbols() for consistency.
//		Renamed SymbolIsVisible() to SymbolsAreVisible() for consistency.
//		Added SetCurveStyle().
//		Reordered arguments to ShowX/YErrors() for consistency.
//		Fixed bug so RemoveCurve() works for index > color count.
//		Added ShouldIgnoreCurveChanged() so redrawing can be turned off temporarily.
//		Fixed bug in ResetScaling() so axes are forced to be linear if all data <= 0.
//		GetX/YScaling() and GetFrame*() are now const.
//		Removed SetX/YAxisLinear() in favor of new SetX/YScaling() that takes additional
//			"linear" argument.  Because of this additional argument, SetX/YScaling()
//			requires absolute values, not exponents, when specifying log scale.
//		Removed restrictions on how far you can zoom in.
//		Optimized drawing of curves.
//		Fixed plus and cross symbols so they are symmetric.
//		Vectors are drawn with arrow heads.
//		Renamed *Scaling() to *Scale().
//		Removed Get/SetDefaultX/YRange() since it was useless.
//		Added version of GetFrameGeometry() that returns JRect.
//		Removed SetSmoothDivisor() since it was useless.
//		Dragging cursors is now much faster.
//		Eliminated PWRefreshUpdate() because it was never used, and JX does it automatically.
//		Fixed bug in FindRange() so it doesn't assert() if min == max.
//		Fixed PWGetDragPainter() to return JBoolean.
//	JX2DPlotWidget:
//		Added functions for printing to EPS.
//		Added right button pop-up menu in legend to change curve options.
//		Promoted GetCursorMenu() to public and made it const.
//		Added GetOptionsMenu().
//		Added second ctor that allows sharing menus with other JX2DPlotWidget.
//	JX2DPlotScaleDialog:
//		Rearranged ctor and GetScaleValues() arguments for consistency.
//		Complains about negative limits when using a log scale.
//	JX2DCurveOptionsDialog:
//		"Show curve" is now a separate checkbox.
//		Only allows lines for functions, since nothing else makes sense.
//		Hides error bar checkboxes if there is no error data.
//	JX2DPlotLabelDialog:
//		GetFontName() returns JString instead of JString&.
//	JX2DCursorMarkTableDir:
//		Read/WriteSetup() are no longer virtual.
//		Window's close action is now kDeactivateDirector.
//	JX2DCursorMarkTable:
//		Renamed TableDrawArea() to Print().

// version 0.6.3:
//	*** Increment version numbers of all files that store plot setup information.
//	JX2DPlotWidget:
//		Fixed PWXRead/WriteSetup() to include a version number.
//		Saves itsPSPrintName as part of setup.
//	JXCursorMarkTableDir:
//		Fixed Read/WriteSetup() to include a version number.

// version 0.6.0:
// J2DPlotWidget
//		Added MoveCursor().
//		Added Print() from J2DPlotWidget.  Now takes JPagePrinter instead of
//			JPSPrinter.
//		PWGetWidth/Height() are no longer virtual.  Instead, they call
//			PWGetGUIWidth/Height() (which are virtual) if not printing.
//		Created pure virtual PrintMarks().
//		Made PWDraw() virtual and added const JRect& argument.
//		Mouse cursor now changes when it is over a plot cursor.
//		Plot cursors no longer move when the plot is zoomed.
//		No longer redraws plot while dragging if the cursor itself didn't move.
//		Always saves cursor positions, even if the cursors are not visible.
//		Fixed clipping so cursor values are displayed at bottom of the widget.
//		Fixed clipping so bottom and right pixels are not lost on plot.
//		PWRead/WriteSetup() is no longer virtual.  Call PWXRead/WriteSetup() in
//			JX2DPlotWidget.
// JX2DPlotWidget:
//		Renamed DrawMarks() to PrintMarks() to avoid conflict with
//			J2DPlotWidget::DrawMarks().
//		Moved Print() to J2DPlotWidget.
//		Removed JX2DPlotWidget::DrawContents() and made PWDraw() virtual instead.
//		Created PWXRead/WriteSetup().
// JXCursorMarkTable:
//		Changed prototype of TableDrawArea() to avoid hiding partial page flag
//			inside rect argument.
// JXPlotRangeDialog:
//		When opened, displays current range, if any.
//		Added Clear button to clear the range and close the dialog.

#endif
