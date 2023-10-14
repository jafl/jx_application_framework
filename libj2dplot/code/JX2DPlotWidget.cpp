/*******************************************************************************
 JX2DPlotWidget.cpp

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DPlotWidget.h"
#include "JX2DPlotLabelDialog.h"
#include "JX2DPlotScaleDialog.h"
#include "JX2DPlotRangeDialog.h"
#include "JX2DCurveOptionsDialog.h"
#include "JX2DCursorMarkTableDir.h"
#include "JX2DCursorMarkTable.h"
#include "JX2DPlotEPSPrinter.h"
#include "J2DPlotDataBase.h"

#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXFontNameMenu.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXPSPrinter.h>

#include <jx-af/jx/JXCursor.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jx/jXGlobals.h>

#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 3;	// must begin with digit >= 2

	// version 2: added version number and itsPSPrintName
	// version 3: added itsEPSPlotName, itsEPSPlotBounds, and itsEPSMarksName

// Options menu

static const JUtf8Byte* kOptionsMenuStr =
	"    Change scale..."
	"  | Reset scale"
	"%l| Show frame  %b"
	"  | Show grid   %b"
	"  | Show legend %b"
	"%l| Copy scale to range"
	"  | Change range..."
	"  | Clear range"
	"%l| Change labels..."
	"  | Change curve options..."
	"  | Remove curve";

enum
{
	kScaleCmd = 1,
	kResetScaleCmd,
	kShowFrameCmd,
	kShowGridCmd,
	kShowLegendCmd,
	kCopyScaleToRangeCmd,
	kChangeRangeCmd,
	kClearRangeCmd,
	kChangeLabelsCmd,
	kCurveOptionsCmd,
	kRemoveCurveCmd
};

// Cursor menu

static const JUtf8Byte* kCursorMenuStr =
	"    X cursor     %b"
	"  | Y cursor     %b"
	"  | Dual cursors %b"
	"%l| Mark cursor"
	"  | Mark all visible cursors"
	"  | Remove mark"
	"  | Remove all marks"
	"%l| Show mark window";

enum
{
	kXCursorCmd = 1,
	kYCursorCmd,
	kDualCursorsCmd,
	kMarkCursorCmd,
	kMarkAllVisCursorsCmd,
	kRemoveMarkIndex,
	kRemoveAllMarksCmd,
	kShowMarkWindowCmd
};

// Curve Options menu (popup)

static const JUtf8Byte* kCurveOptionsMenuStr =
	"    Visible         %b"
	"%l| Show all"
	"  | Hide others"
	"%l| Points          %r"
	"  | Symbols         %r"
	"  | Lines           %r"
	"  | Lines & symbols %r"
	"%l| X errors        %b"
	"  | Y errors        %b";

enum
{
	kToggleCurveVisibleCmd = 1,
	kShowAllCurvesCmd, kHideAllOtherCurvesCmd,
	kPointsCmd, kSymbolsCmd, kLinesCmd, kLinesSymbolsCmd,
	kXErrorsCmd, kYErrorsCmd
};

/*******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotWidget::JX2DPlotWidget
	(
	JXMenuBar*			menuBar,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	J2DPlotWidget(JColorManager::GetBlackColor(),
				  JColorManager::GetWhiteColor(),
				  JColorManager::GetGrayColor(70),
				  JColorManager::GetRedColor()),
	itsIsSharingMenusFlag(false)
{
	itsOptionsMenu = menuBar->AppendTextMenu(JGetString("OptionsMenuTitle::JX2DPlotWidget"));
	assert( itsOptionsMenu != nullptr );
	itsOptionsMenu->SetMenuItems(kOptionsMenuStr);
	itsOptionsMenu->SetUpdateAction(JXMenu::kDisableNone);

	itsRemoveCurveMenu = jnew JXTextMenu(itsOptionsMenu, kRemoveCurveCmd, menuBar);
	assert( itsRemoveCurveMenu != nullptr );
	itsRemoveCurveMenu->SetUpdateAction(JXMenu::kDisableNone);

	itsCursorMenu = menuBar->AppendTextMenu(JGetString("CursorMenuTitle::JX2DPlotWidget"));
	itsCursorMenu->SetMenuItems(kCursorMenuStr);
	itsCursorMenu->SetUpdateAction(JXMenu::kDisableNone);

	itsMarkMenu = jnew JXTextMenu(itsCursorMenu, kRemoveMarkIndex, menuBar);
	assert( itsMarkMenu != nullptr );
	itsMarkMenu->SetUpdateAction(JXMenu::kDisableNone);

	itsCurveOptionsMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsCurveOptionsMenu != nullptr );
	itsCurveOptionsMenu->SetToHiddenPopupMenu();
	itsCurveOptionsMenu->SetMenuItems(kCurveOptionsMenuStr);
	itsCurveOptionsMenu->SetUpdateAction(JXMenu::kDisableNone);

	JX2DPlotWidgetX();
}

JX2DPlotWidget::JX2DPlotWidget
	(
	JX2DPlotWidget*		plot,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	J2DPlotWidget(JColorManager::GetBlackColor(),
				  JColorManager::GetWhiteColor(),
				  JColorManager::GetGrayColor(50),
				  JColorManager::GetRedColor()),
	itsOptionsMenu(plot->itsOptionsMenu),
	itsRemoveCurveMenu(plot->itsRemoveCurveMenu),
	itsCursorMenu(plot->itsCursorMenu),
	itsMarkMenu(plot->itsMarkMenu),
	itsIsSharingMenusFlag(true),
	itsCurveOptionsMenu(plot->itsCurveOptionsMenu)
{
	JX2DPlotWidgetX();
	plot->itsIsSharingMenusFlag = true;
}

// private

void
JX2DPlotWidget::JX2DPlotWidgetX()
{
	SetBorderWidth(kJXDefaultBorderWidth);
	SetBackColor(JColorManager::GetWhiteColor());
	AddColor(JColorManager::GetRedColor());
	AddColor(JColorManager::GetBlueColor());
	AddColor(JColorManager::GetMagentaColor());
	AddColor(JColorManager::GetBrownColor());
	AddColor(JColorManager::GetLightBlueColor());

	itsMarkDir = jnew JX2DCursorMarkTableDir(GetWindow()->GetDirector(), this);
	assert(itsMarkDir != nullptr);

	JXDisplay* display = GetDisplay();
	itsDragXCursor     = JXGetDragVertLineCursor(display);
	itsDragYCursor     = JXGetDragHorizLineCursor(display);

	itsPSPrinter  = nullptr;
	itsEPSPrinter = nullptr;

	WantInput(true, true);
	ListenTo(this);		// update "Remove mark" menu

	itsOptionsMenu->AttachHandlers(this,
		&JX2DPlotWidget::UpdateOptionsMenu,
		&JX2DPlotWidget::HandleOptionsMenu);

	itsRemoveCurveMenu->AttachHandlers(this,
		&JX2DPlotWidget::UpdateRemoveCurveMenu,
		&JX2DPlotWidget::HandleRemoveCurveMenu);

	itsCursorMenu->AttachHandlers(this,
		&JX2DPlotWidget::UpdateCursorMenu,
		&JX2DPlotWidget::HandleCursorMenu);

	itsMarkMenu->AttachHandlers(this,
		&JX2DPlotWidget::UpdateMarkMenu,
		&JX2DPlotWidget::HandleMarkMenu);

	itsCurveOptionsMenu->AttachHandlers(this,
		&JX2DPlotWidget::UpdateCurveOptionsMenu,
		&JX2DPlotWidget::HandleCurveOptionsMenu);
}

/*******************************************************************************
 Destructor

 ******************************************************************************/

JX2DPlotWidget::~JX2DPlotWidget()
{
}

/*******************************************************************************
 Draw

 ******************************************************************************/

void
JX2DPlotWidget::Draw
	(
	JXWindowPainter& p,
	const JRect& rect
	)
{
	PWDraw(p, rect);
}

/*******************************************************************************
 DrawBorder

 ******************************************************************************/

void
JX2DPlotWidget::DrawBorder
	(
	JXWindowPainter& p,
	const JRect& frame
	)
{
	JXDrawDownFrame(p, frame, GetBorderWidth());
}

/*******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
JX2DPlotWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	PWHandleKeyPress(c, modifiers.shift(), modifiers.meta(), false);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton)
	{
		PWHandleMouseDown(pt, clickCount);
	}
	else if (button == kJXRightButton && GetLegendIndex(pt, &itsCurveOptionsIndex))
	{
		itsCurveOptionsMenu->PopUp(this, pt, buttonStates, modifiers);
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (buttonStates.left())
	{
		PWHandleMouseDrag(pt);
	}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton)
	{
		PWHandleMouseUp(pt, modifiers.meta());
	}
}

/*******************************************************************************
 BoundsResized

 ******************************************************************************/

void
JX2DPlotWidget::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidget::BoundsResized(dw,dh);
	PWBoundsChanged();
}

/*******************************************************************************
 PWGetGUIWidth (protected)

 ******************************************************************************/

JSize
JX2DPlotWidget::PWGetGUIWidth()
	const
{
	return GetApertureWidth();
}

/*******************************************************************************
 PWGetGUIHeight (protected)

 ******************************************************************************/

JSize
JX2DPlotWidget::PWGetGUIHeight()
	const
{
	return GetApertureHeight();
}

/*******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(kCurveAdded))
	{
		UpdateRemoveCurveMenu();
	}
	else if (sender == this && message.Is(kCursorMarked))
	{
		UpdateMarkMenu();
	}

	J2DPlotWidget::Receive(sender,message);
	JXWidget::Receive(sender, message);
}

/*******************************************************************************
 UpdateOptionsMenu

 ******************************************************************************/

void
JX2DPlotWidget::UpdateOptionsMenu()
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	if (FrameIsVisible())
	{
		itsOptionsMenu->CheckItem(kShowFrameCmd);
	}

	if (GridIsVisible())
	{
		itsOptionsMenu->CheckItem(kShowGridCmd);
	}

	if (LegendIsVisible())
	{
		itsOptionsMenu->CheckItem(kShowLegendCmd);
	}
	UpdateRemoveCurveMenu();
}

/*******************************************************************************
 HandleOptionsMenu

 ******************************************************************************/

void
JX2DPlotWidget::HandleOptionsMenu
	(
	const JIndex index
	)
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	if (index == kCurveOptionsCmd)
	{
		ChangeCurveOptions(1);
	}

	else if (index == kScaleCmd)
	{
		ChangeScale();
	}

	else if (index == kResetScaleCmd)
	{
		ResetScale();
	}

	else if (index == kCopyScaleToRangeCmd)
	{
		CopyScaleToRange();
	}

	else if (index == kChangeRangeCmd)
	{
		ChangeRange();
	}

	else if (index == kClearRangeCmd)
	{
		ClearRange();
	}

	else if (index == kChangeLabelsCmd)
	{
		ChangeLabels(kNoLabel);
	}

	else if (index == kShowFrameCmd)
	{
		if (FrameIsVisible())
		{
			ShowFrame(false);
		}
		else
		{
			ShowFrame(true);
		}

	}

	else if (index == kShowGridCmd)
	{
		if (GridIsVisible())
		{
			ShowGrid(false);
		}
		else
		{
			ShowGrid(true);
		}
	}

	else if (index == kShowLegendCmd)
	{
		if (LegendIsVisible())
		{
			ShowLegend(false);
		}
		else
		{
			ShowLegend(true);
		}
	}
}

/*******************************************************************************
 ChangeLabels

 ******************************************************************************/

void
JX2DPlotWidget::ChangeLabels
	(
	const LabelSelection selection
	)
{
	auto* dlog =
		jnew JX2DPlotLabelDialog(GetTitle(), GetXLabel(), GetYLabel(),
								 GetFontName(), GetFontSize(), selection);
	assert (dlog != nullptr);

	if (dlog->DoDialog())
	{
		JString title, xLabel, yLabel;
		dlog->GetLabels(&title, &xLabel, &yLabel);
		SetTitle(title);
		SetXLabel(xLabel);
		SetYLabel(yLabel);

		const JString fontName = dlog->GetFontName();
		SetFontName(fontName);
		SetFontSize(dlog->GetFontSize());
	}
}

/*******************************************************************************
 ChangeScale

 ******************************************************************************/

void
JX2DPlotWidget::ChangeScale
	(
	const bool editXAxis
	)
{
	JFloat xmin, xmax, xinc, ymin, ymax, yinc;
	GetXScale(&xmin, &xmax, &xinc);
	GetYScale(&ymin, &ymax, &yinc);

	auto* dlog =
		jnew JX2DPlotScaleDialog(xmin, xmax, xinc, XAxisIsLinear(),
								 ymin, ymax, yinc, YAxisIsLinear(),
								 editXAxis);
	assert( dlog != nullptr );
	if (dlog->DoDialog())
	{
		bool xLinear, yLinear;

		dlog->GetScaleValues(&xmin, &xmax, &xinc, &xLinear, &ymin, &ymax, &yinc, &yLinear);
		SetXScale(xmin, xmax, (xLinear ? xinc : -1.0), xLinear);
		SetYScale(ymin, ymax, (yLinear ? yinc : -1.0), yLinear);
	}
}

/*******************************************************************************
 ChangeScale

 ******************************************************************************/

void
JX2DPlotWidget::ChangeScale()
{
	ChangeScale(true);
}

/*******************************************************************************
 ChangeRange (private)

 ******************************************************************************/

void
JX2DPlotWidget::ChangeRange()
{
	JFloat xmax, xmin, ymax, ymin;
	if (!GetRange(&xmin, &xmax, &ymin, &ymax))
	{
		JFloat xinc, yinc;
		GetXScale(&xmin, &xmax, &xinc);
		GetYScale(&ymin, &ymax, &yinc);
	}

	auto* dlog = jnew JX2DPlotRangeDialog(xmax, xmin, ymax, ymin);
	assert( dlog != nullptr );

	if (dlog->DoDialog())
	{
		if (dlog->GetRangeValues(&xmax, &xmin, &ymax, &ymin))
		{
			SetRange(xmin, xmax, ymin, ymax);
		}
		else
		{
			ClearRange();
		}
	}
}

/*******************************************************************************
 PWRefreshRect (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::PWRefreshRect
	(
	const JRect& rect
	)
{
	RefreshRect(rect);
}

/*******************************************************************************
 PWRedraw (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::PWRedraw()
{
	Redraw();
}

/*******************************************************************************
 PWRedrawRect (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::PWRedrawRect
	(
	const JRect& rect
	)
{
	RedrawRect(rect);
}

/*******************************************************************************
 PWForceRefresh (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::PWForceRefresh()
{
	Refresh();
}

/*******************************************************************************
 PWCreateDragInsidePainter (virtual protected)

 ******************************************************************************/

JPainter*
JX2DPlotWidget::PWCreateDragInsidePainter()
{
	return CreateDragInsidePainter();
}

/*******************************************************************************
 PWGetDragPainter (virtual protected)

 ******************************************************************************/

bool
JX2DPlotWidget::PWGetDragPainter
	(
	JPainter** p
	)
	const
{
	return GetDragPainter(p);
}

/*******************************************************************************
 PWDeleteDragPainter (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::PWDeleteDragPainter()
{
	DeleteDragPainter();
}

/*******************************************************************************
 ChangeCurveOptions (private)

 ******************************************************************************/

void
JX2DPlotWidget::ChangeCurveOptions()
{
	ChangeCurveOptions(itsCurrentCurve);
}

/*******************************************************************************
 ChangeCurveOptions

 ******************************************************************************/

void
JX2DPlotWidget::ChangeCurveOptions
	(
	const JIndex index
	)
{
	JArray<bool> hasXErrors, hasYErrors, isFunction, isScatter;

	const JSize count = GetCurveCount();
	for (JIndex i=1; i<=count; i++)
	{
		const J2DPlotDataBase* curve = GetCurve(i);
		hasXErrors.AppendElement(curve->HasXErrors());
		hasYErrors.AppendElement(curve->HasYErrors());
		isFunction.AppendElement(curve->IsFunction());
		isScatter.AppendElement(curve->GetType() == J2DPlotDataBase::kScatterPlot);
	}

	auto* dlog =
		jnew JX2DCurveOptionsDialog(GetCurveInfoArray(),
									hasXErrors, hasYErrors, isFunction, isScatter, index);
	assert( dlog != nullptr );

	if (dlog->DoDialog())
	{
		SetCurveInfoArray(dlog->GetCurveInfoArray());
	}
}

/*******************************************************************************
 UpdateRemoveCurveMenu

 ******************************************************************************/

void
JX2DPlotWidget::UpdateRemoveCurveMenu()
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	itsRemoveCurveMenu->RemoveAllItems();

	const JSize count = GetCurveCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsRemoveCurveMenu->AppendItem(GetCurveName(i));
		if (CurveIsProtected(i))
		{
			itsRemoveCurveMenu->DisableItem(i);
		}
	}
}

/*******************************************************************************
 HandleRemoveCurveMenu

 ******************************************************************************/

void
JX2DPlotWidget::HandleRemoveCurveMenu
	(
	const JIndex index
	)
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	RemoveCurve(index);
}

/*******************************************************************************
 UpdateCursorMenu

 ******************************************************************************/

void
JX2DPlotWidget::UpdateCursorMenu()
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	if (XCursorVisible())
	{
		itsCursorMenu->CheckItem(kXCursorCmd);
	}

	if (YCursorVisible())
	{
		itsCursorMenu->CheckItem(kYCursorCmd);
	}

	if (DualCursorsVisible())
	{
		itsCursorMenu->CheckItem(kDualCursorsCmd);
	}

	itsCursorMenu->SetItemEnabled(kMarkCursorCmd, CursorIsSelected());
	itsCursorMenu->SetItemEnabled(kMarkAllVisCursorsCmd,
								 XCursorVisible() || YCursorVisible());
	itsCursorMenu->SetItemEnabled(kRemoveAllMarksCmd,
								GetXMarkCount() > 0 || GetYMarkCount() > 0);
}

/*******************************************************************************
 HandleCursorMenu

 ******************************************************************************/

void
JX2DPlotWidget::HandleCursorMenu
	(
	const JIndex index
	)
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	if (index == kXCursorCmd)
	{
		ToggleXCursor();
	}
	else if (index == kYCursorCmd)
	{
		ToggleYCursor();
	}
	else if (index == kDualCursorsCmd)
	{
		ToggleDualCursors();
	}
	else if (index == kMarkCursorCmd)
	{
		MarkCurrentCursor();
	}
	else if (index == kMarkAllVisCursorsCmd)
	{
		MarkAllVisibleCursors();
	}
	else if (index == kRemoveAllMarksCmd)
	{
		RemoveAllMarks();
	}
	else if (index == kShowMarkWindowCmd)
	{
		itsMarkDir->Activate();
	}
}

/*******************************************************************************
 UpdateMarkMenu

 ******************************************************************************/

void
JX2DPlotWidget::UpdateMarkMenu()
{
JIndex i;

	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	itsMarkMenu->RemoveAllItems();

	const JSize xCount = GetXMarkCount();
	const JSize yCount = GetYMarkCount();

	for (i=1; i<=xCount; i++)
	{
		JString str("x");
		str += JString((JUInt64) i);
		itsMarkMenu->AppendItem(str);
	}

	if (xCount > 0 && yCount > 0)
	{
		itsMarkMenu->ShowSeparatorAfter(xCount);
	}

	for (i=1; i<=yCount; i++)
	{
		JString str("y");
		str += JString((JUInt64) i);
		itsMarkMenu->AppendItem(str);
	}
}

/*******************************************************************************
 HandleMarkMenu

 ******************************************************************************/

void
JX2DPlotWidget::HandleMarkMenu
	(
	const JIndex index
	)
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	const JSize xCount = GetXMarkCount();
	if (index > xCount)
	{
		RemoveYMark(index - xCount);
	}
	else
	{
		RemoveXMark(index);
	}
}

/*******************************************************************************
 UpdateCurveOptionsMenu

 ******************************************************************************/

void
JX2DPlotWidget::UpdateCurveOptionsMenu()
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	const J2DPlotDataBase* curve = GetCurve(itsCurveOptionsIndex);

	// visibility

	if (CurveIsVisible(itsCurveOptionsIndex))
	{
		itsCurveOptionsMenu->CheckItem(kToggleCurveVisibleCmd);
	}

	// style

	JIndex style;
	if (curve->GetType() != J2DPlotDataBase::kScatterPlot)
	{
		style = (SymbolsAreVisible(itsCurveOptionsIndex) ? kSymbolsCmd : kPointsCmd);
		itsCurveOptionsMenu->EnableItem(kPointsCmd);
		itsCurveOptionsMenu->EnableItem(kSymbolsCmd);
		itsCurveOptionsMenu->DisableItem(kLinesCmd);
		itsCurveOptionsMenu->DisableItem(kLinesSymbolsCmd);
	}
	else if (curve->IsFunction())
	{
		style = kLinesCmd;
		itsCurveOptionsMenu->DisableItem(kPointsCmd);
		itsCurveOptionsMenu->DisableItem(kSymbolsCmd);
		itsCurveOptionsMenu->EnableItem(kLinesCmd);
		itsCurveOptionsMenu->DisableItem(kLinesSymbolsCmd);
	}
	else
	{
		const bool lines   = LinesAreVisible(itsCurveOptionsIndex);
		const bool symbols = SymbolsAreVisible(itsCurveOptionsIndex);
		if (lines && symbols)
		{
			style = kLinesSymbolsCmd;
		}
		else if (lines)
		{
			style = kLinesCmd;
		}
		else if (symbols)
		{
			style = kSymbolsCmd;
		}
		else
		{
			style = kPointsCmd;
		}

		itsCurveOptionsMenu->EnableItem(kPointsCmd);
		itsCurveOptionsMenu->EnableItem(kSymbolsCmd);
		itsCurveOptionsMenu->EnableItem(kLinesCmd);
		itsCurveOptionsMenu->EnableItem(kLinesSymbolsCmd);
	}
	itsCurveOptionsMenu->CheckItem(style);

	// error bars

	itsCurveOptionsMenu->SetItemEnabled(kXErrorsCmd, curve->HasXErrors());
	if (curve->HasXErrors() && XErrorsAreVisible(itsCurveOptionsIndex))
	{
		itsCurveOptionsMenu->CheckItem(kXErrorsCmd);
	}

	itsCurveOptionsMenu->SetItemEnabled(kYErrorsCmd, curve->HasYErrors());
	if (curve->HasYErrors() && YErrorsAreVisible(itsCurveOptionsIndex))
	{
		itsCurveOptionsMenu->CheckItem(kYErrorsCmd);
	}
}

/*******************************************************************************
 HandleCurveOptionsMenu

 ******************************************************************************/

void
JX2DPlotWidget::HandleCurveOptionsMenu
	(
	const JIndex index
	)
{
	if (itsIsSharingMenusFlag && !HasFocus())
	{
		return;
	}

	if (index == kToggleCurveVisibleCmd)
	{
		ShowCurve(itsCurveOptionsIndex, !CurveIsVisible(itsCurveOptionsIndex));
	}

	else if (index == kShowAllCurvesCmd)
	{
		ShowAllCurves();
	}
	else if (index == kHideAllOtherCurvesCmd)
	{
		HideAllOtherCurves(itsCurveOptionsIndex);
	}

	else if (index == kPointsCmd)
	{
		SetCurveStyle(itsCurveOptionsIndex, false, false);
	}
	else if (index == kSymbolsCmd)
	{
		SetCurveStyle(itsCurveOptionsIndex, false, true);
	}
	else if (index == kLinesCmd)
	{
		SetCurveStyle(itsCurveOptionsIndex, true, false);
	}
	else if (index == kLinesSymbolsCmd)
	{
		SetCurveStyle(itsCurveOptionsIndex, true, true);
	}

	else if (index == kXErrorsCmd)
	{
		ShowXErrors(itsCurveOptionsIndex, !XErrorsAreVisible(itsCurveOptionsIndex));
	}
	else if (index == kYErrorsCmd)
	{
		ShowYErrors(itsCurveOptionsIndex, !YErrorsAreVisible(itsCurveOptionsIndex));
	}
}

/*******************************************************************************
 ProtectionChanged

 ******************************************************************************/

void
JX2DPlotWidget::ProtectionChanged()
{
//	UpdateRemoveCurveMenu();
}

/******************************************************************************
 SetPSPrinter

	Call this to provide a JXPSPrinter object for this plot.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

void
JX2DPlotWidget::SetPSPrinter
	(
	JXPSPrinter* p
	)
{
	itsPSPrinter = p;
}

/******************************************************************************
 HandlePSPageSetup

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JX2DPlotWidget::HandlePSPageSetup()
{
	assert( itsPSPrinter != nullptr );

	itsPSPrinter->EditUserPageSetup();
}

/******************************************************************************
 PrintPS

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JX2DPlotWidget::PrintPS()
{
	assert( itsPSPrinter != nullptr );

	itsPSPrinter->SetFileName(itsPSPrintName);
	if (itsPSPrinter->ConfirmUserPrintSetup())
	{
		itsPSPrintName = itsPSPrinter->GetFileName();
		Print(*itsPSPrinter);
	}
}

/******************************************************************************
 GetMarksHeight (virtual protected)

	For PostScript printing.  Returns the height of the table.

 ******************************************************************************/

JCoordinate
JX2DPlotWidget::GetMarksHeight()
	const
{
	return ((GetXMarkCount() > 0 || GetYMarkCount() > 0) ?
			(itsMarkDir->GetTable())->GetBoundsHeight() : 0);
}

/******************************************************************************
 PrintMarks (virtual protected)

	PostScript.  Returns false if printing was cancelled.

 ******************************************************************************/

bool
JX2DPlotWidget::PrintMarks
	(
	JPagePrinter&	p,
	const bool	putOnSamePage,
	const JRect&	partialPageRect
	)
{
	return (itsMarkDir->GetTable())->Print(p, putOnSamePage, partialPageRect);
}

/******************************************************************************
 SetEPSPrinter

	Call this to provide a JX2DPlotEPSPrinter object for this plot.
	This object does *not* take ownership of the printer object.

 ******************************************************************************/

void
JX2DPlotWidget::SetEPSPrinter
	(
	JX2DPlotEPSPrinter* p
	)
{
	if (itsEPSPrinter != nullptr)
	{
		StopListening(itsEPSPrinter);
	}
	itsEPSPrinter = p;
}

/******************************************************************************
 PrintPlotEPS

	You must call SetEPSPrinter() before using this routine.

 ******************************************************************************/

void
JX2DPlotWidget::PrintPlotEPS()
{
	assert( itsEPSPrinter != nullptr );

	itsEPSPrinter->SetOutputFileName(itsEPSPlotName);
	itsEPSPrinter->UsePlotSetup(true);
	if (!itsEPSPlotBounds.IsEmpty())
	{
		itsEPSPrinter->SetPlotBounds(itsEPSPlotBounds);
	}
	if (itsEPSPrinter->ConfirmUserPrintSetup())
	{
		itsEPSPlotName   = itsEPSPrinter->GetOutputFileName();
		itsEPSPlotBounds = itsEPSPrinter->GetPlotBounds();
		Print(*itsEPSPrinter, itsEPSPlotBounds);
	}
}

/******************************************************************************
 PrintMarksEPS

	You must call SetEPSPrinter() before using this routine.

 ******************************************************************************/

void
JX2DPlotWidget::PrintMarksEPS()
{
	assert( itsEPSPrinter != nullptr );

	itsEPSPrinter->SetOutputFileName(itsEPSMarksName);
	itsEPSPrinter->UsePlotSetup(false);
	if (itsEPSPrinter->ConfirmUserPrintSetup())
	{
		itsEPSMarksName = itsEPSPrinter->GetOutputFileName();
		itsMarkDir->GetTable()->Print(*itsEPSPrinter);
	}
}

/******************************************************************************
 PWXReadSetup

	This uses assert() to check the version number because we cannot use
	a marker to skip the data (since it contains objects), so it is the
	client's responsibility to check whether or not the file can be
	read (via a higher level version number).

 ******************************************************************************/

void
JX2DPlotWidget::PWXReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers = 0;
	input >> std::ws;
	const char c1 = input.peek();
	if ('2' <= c1 && c1 <= '9')			// version 1 => leave it for J2DPlotWidget
	{
		input >> vers;
		assert( vers <= kCurrentSetupVersion );
	}

	J2DPlotWidget::PWReadSetup(input);

	itsMarkDir->ReadSetup(input);
	bool active;
	input >> JBoolFromString(active);
	if (active)
	{
		itsMarkDir->Activate();
	}
	else
	{
		itsMarkDir->Deactivate();
	}

	if (vers >= 2)
	{
		input >> itsPSPrintName;
	}

	if (vers >= 3)
	{
		input >> itsEPSPlotName >> itsEPSPlotBounds;
		input >> itsEPSMarksName;
	}
}

/******************************************************************************
 PWXWriteSetup

 ******************************************************************************/

void
JX2DPlotWidget::PWXWriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	output << ' ';
	J2DPlotWidget::PWWriteSetup(output);

	output << ' ';
	itsMarkDir->WriteSetup(output);
	output << ' ' << JBoolToString(itsMarkDir->IsActive());

	output << ' ' << itsPSPrintName;
	output << ' ' << itsEPSPlotName;
	output << ' ' << itsEPSPlotBounds;
	output << ' ' << itsEPSMarksName;

	output << ' ';
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	PWDisplayCursor(GetMouseCursor(pt));
}

/******************************************************************************
 PWDisplayCursor (virtual protected)

 ******************************************************************************/

void
JX2DPlotWidget::PWDisplayCursor
	(
	const MouseCursor cursor
	)
{
	if (cursor == kZoomCursor)
	{
		DisplayCursor(kJXCrosshairCursor);
	}
	else if (cursor == kDragXCursor)
	{
		DisplayCursor(itsDragXCursor);
	}
	else if (cursor == kDragYCursor)
	{
		DisplayCursor(itsDragYCursor);
	}
	else
	{
		assert( cursor == kDblClickChangeCursor );
		DisplayCursor(kJXDefaultCursor);
	}
}
