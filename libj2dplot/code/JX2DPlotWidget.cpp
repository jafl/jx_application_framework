/*******************************************************************************
 JX2DPlotWidget.cpp

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JX2DPlotWidget.h>
#include <JX2DPlotLabelDialog.h>
#include <JX2DPlotScaleDialog.h>
#include <JX2DPlotRangeDialog.h>
#include <JX2DCurveOptionsDialog.h>
#include <JX2DCursorMarkTableDir.h>
#include <JX2DCursorMarkTable.h>
#include <JX2DPlotEPSPrinter.h>
#include <JPlotDataBase.h>

#include <JXWindowPainter.h>
#include <JXWindowDirector.h>
#include <JXDragPainter.h>
#include <JXColormap.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <JXFontSizeMenu.h>
#include <JXFontNameMenu.h>
#include <JXWindow.h>
#include <JXDisplay.h>
#include <JXPSPrinter.h>

#include <JXCursor.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jXGlobals.h>

#include <JString.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 3;	// must begin with digit >= 2

	// version 2: added version number and itsPSPrintName
	// version 3: added itsEPSPlotName, itsEPSPlotBounds, and itsEPSMarksName

// Options menu

static const JCharacter* kOptionsMenuTitleStr = "Options";
static const JCharacter* kOptionsMenuStr =
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

static const JCharacter* kCursorMenuTitleStr = "Cursors";
static const JCharacter* kCursorMenuStr =
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

static const JCharacter* kCurveOptionsMenuStr =
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
	J2DPlotWidget(GetColormap()->GetBlackColor(),
				  GetColormap()->GetWhiteColor(),
				  GetColormap()->GetGrayColor(70),
				  GetColormap()->GetRedColor())
{
	JX2DPlotWidgetX();

	itsOptionsMenu = menuBar->AppendTextMenu(kOptionsMenuTitleStr);
	assert( itsOptionsMenu != NULL );
	itsOptionsMenu->SetMenuItems(kOptionsMenuStr);
	itsOptionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsOptionsMenu);

	itsRemoveCurveMenu = new JXTextMenu(itsOptionsMenu, kRemoveCurveCmd, menuBar);
	assert( itsRemoveCurveMenu != NULL );
	itsRemoveCurveMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsRemoveCurveMenu);

	itsCursorMenu = menuBar->AppendTextMenu(kCursorMenuTitleStr);
	itsCursorMenu->SetMenuItems(kCursorMenuStr);
	itsCursorMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCursorMenu);

	itsMarkMenu = new JXTextMenu(itsCursorMenu, kRemoveMarkIndex, menuBar);
	assert( itsMarkMenu != NULL );
	itsMarkMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsMarkMenu);

	itsCurveOptionsMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsCurveOptionsMenu != NULL );
	itsCurveOptionsMenu->SetToHiddenPopupMenu();
	itsCurveOptionsMenu->SetMenuItems(kCurveOptionsMenuStr);
	itsCurveOptionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCurveOptionsMenu);

	itsIsSharingMenusFlag = kJFalse;
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
	J2DPlotWidget(GetColormap()->GetBlackColor(),
				  GetColormap()->GetWhiteColor(),
				  GetColormap()->GetGrayColor(50),
				  GetColormap()->GetRedColor())
{
	JX2DPlotWidgetX();

	itsOptionsMenu	= plot->itsOptionsMenu;
	ListenTo(itsOptionsMenu);

	itsRemoveCurveMenu	= plot->itsRemoveCurveMenu;
	ListenTo(itsRemoveCurveMenu);

	itsCursorMenu	= plot->itsCursorMenu;
	ListenTo(itsCursorMenu);

	itsMarkMenu	= plot->itsMarkMenu;
	ListenTo(itsMarkMenu);

	itsCurveOptionsMenu	= plot->itsCurveOptionsMenu;
	ListenTo(itsCurveOptionsMenu);

	itsIsSharingMenusFlag       = kJTrue;
	plot->itsIsSharingMenusFlag = kJTrue;
}

// private

void
JX2DPlotWidget::JX2DPlotWidgetX()
{
	SetBorderWidth(kJXDefaultBorderWidth);
	JXColormap* colormap = GetColormap();
	SetBackColor(colormap->GetWhiteColor());
	AddColor(colormap->GetRedColor());
	AddColor(colormap->GetBlueColor());
	AddColor(colormap->GetMagentaColor());
	AddColor(colormap->GetBrownColor());
	AddColor(colormap->GetLightBlueColor());

	itsPlotLabelDialog		= NULL;
	itsPlotScaleDialog		= NULL;
	itsPlotRangeDialog		= NULL;
	itsCurveOptionsDialog	= NULL;

	itsMarkDir = new JX2DCursorMarkTableDir(GetWindow()->GetDirector(), this);
	assert(itsMarkDir != NULL);

	JXDisplay* display = GetDisplay();
	itsDragXCursor     = JXGetDragVertLineCursor(display);
	itsDragYCursor     = JXGetDragHorizLineCursor(display);

	itsPSPrinter  = NULL;
	itsEPSPrinter = NULL;

	WantInput(kJTrue, kJTrue);
	ListenTo(this);		// update "Remove mark" menu
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
	const int key,
	const JXKeyModifiers& modifiers
	)
{
	PWHandleKeyPress(key, modifiers.shift(), modifiers.meta(), kJFalse);
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
	if (sender == itsOptionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateOptionsMenu();
		}
	else if (sender == itsOptionsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleOptionsMenu(selection->GetIndex());
		}

	else if (sender == itsRemoveCurveMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateRemoveCurveMenu();
		}
	else if (sender == itsRemoveCurveMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleRemoveCurveMenu(selection->GetIndex());
		}

	else if (sender == itsCursorMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCursorMenu();
		}
	else if (sender == itsCursorMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleCursorMenu(selection->GetIndex());
		}

	else if (sender == itsMarkMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMarkMenu();
		}
	else if (sender == itsMarkMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleMarkMenu(selection->GetIndex());
		}

	else if (sender == itsCurveOptionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCurveOptionsMenu();
		}
	else if (sender == itsCurveOptionsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleCurveOptionsMenu(selection->GetIndex());
		}

	else if (sender == itsPlotLabelDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			GetNewLabels();
			}
		itsPlotLabelDialog = NULL;
		}

	else if (sender == itsPlotScaleDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			GetNewScale();
			}
		itsPlotScaleDialog = NULL;
		}

	else if (sender == itsPlotRangeDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			GetNewRange();
			}
		itsPlotRangeDialog = NULL;
		}

	else if (sender == itsCurveOptionsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			GetNewCurveOptions();
			}
		itsCurveOptionsDialog = NULL;
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsPSPrintName = itsPSPrinter->GetFileName();
			Print(*itsPSPrinter);
			}
		StopListening(itsPSPrinter);
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful() && itsPrintEPSPlotFlag)
			{
			itsEPSPlotName   = itsEPSPrinter->GetOutputFileName();
			itsEPSPlotBounds = itsEPSPrinter->GetPlotBounds();
			Print(*itsEPSPrinter, itsEPSPlotBounds);
			}
		else if (info->Successful())
			{
			itsEPSMarksName = itsEPSPrinter->GetOutputFileName();
			(itsMarkDir->GetTable())->JTable::Print(*itsEPSPrinter);
			}
		StopListening(itsEPSPrinter);
		}

	else
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
			ShowFrame(kJFalse);
			}
		else
			{
			ShowFrame(kJTrue);
			}

		}

	else if (index == kShowGridCmd)
		{
		if (GridIsVisible())
			{
			ShowGrid(kJFalse);
			}
		else
			{
			ShowGrid(kJTrue);
			}
		}

	else if (index == kShowLegendCmd)
		{
		if (LegendIsVisible())
			{
			ShowLegend(kJFalse);
			}
		else
			{
			ShowLegend(kJTrue);
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
	assert (itsPlotLabelDialog == NULL);
	itsPlotLabelDialog =
		new JX2DPlotLabelDialog(GetWindow()->GetDirector(),
								GetTitle(), GetXLabel(), GetYLabel(),
								GetFontName(), GetFontSize(), selection);
	assert (itsPlotLabelDialog != NULL);
	ListenTo(itsPlotLabelDialog);
	itsPlotLabelDialog->BeginDialog();
}

/*******************************************************************************
 GetNewLabels

 ******************************************************************************/

void
JX2DPlotWidget::GetNewLabels()
{
	assert( itsPlotLabelDialog != NULL );

	JString title, xLabel, yLabel;
	itsPlotLabelDialog->GetLabels(&title, &xLabel, &yLabel);
	SetTitle(title);
	SetXLabel(xLabel);
	SetYLabel(yLabel);

	const JString fontName = itsPlotLabelDialog->GetFontName();
	SetFontName(fontName);
	SetFontSize(itsPlotLabelDialog->GetFontSize());
}

/*******************************************************************************
 ChangeScale

 ******************************************************************************/

void
JX2DPlotWidget::ChangeScale
	(
	const JBoolean xAxis
	)
{
	assert (itsPlotScaleDialog == NULL);
	JFloat xmin, xmax, xinc, ymin, ymax, yinc;
	GetXScale(&xmin, &xmax, &xinc);
	GetYScale(&ymin, &ymax, &yinc);

	itsPlotScaleDialog =
		new JX2DPlotScaleDialog(GetWindow()->GetDirector(),
								xmin, xmax, xinc, XAxisIsLinear(),
								ymin, ymax, yinc, YAxisIsLinear());
	assert (itsPlotScaleDialog != NULL);
	ListenTo(itsPlotScaleDialog);
	itsPlotScaleDialog->BeginDialog();
	itsPlotScaleDialog->EditXAxis(xAxis);
}

/*******************************************************************************
 ChangeScale

 ******************************************************************************/

void
JX2DPlotWidget::ChangeScale()
{
	ChangeScale(kJTrue);
}

/*******************************************************************************
 GetNewScale

 ******************************************************************************/

void
JX2DPlotWidget::GetNewScale()
{
	assert( itsPlotScaleDialog != NULL );

	JFloat xmin, xmax, xinc, ymin, ymax, yinc;
	JBoolean xLinear, yLinear;

	itsPlotScaleDialog->GetScaleValues(&xmin, &xmax, &xinc, &xLinear, &ymin, &ymax, &yinc, &yLinear);
	SetXScale(xmin, xmax, (xLinear ? xinc : -1.0), xLinear);
	SetYScale(ymin, ymax, (yLinear ? yinc : -1.0), yLinear);
}

/*******************************************************************************
 ChangeRange (private)

 ******************************************************************************/

void
JX2DPlotWidget::ChangeRange()
{
	assert (itsPlotRangeDialog == NULL);
	JFloat xmax, xmin, ymax, ymin;
	if (!GetRange(&xmin, &xmax, &ymin, &ymax))
		{
		JFloat xinc, yinc;
		GetXScale(&xmin, &xmax, &xinc);
		GetYScale(&ymin, &ymax, &yinc);
		}
	itsPlotRangeDialog =
		new JX2DPlotRangeDialog(GetWindow()->GetDirector(),
								xmax, xmin, ymax, ymin);
	assert (itsPlotRangeDialog != NULL);
	ListenTo(itsPlotRangeDialog);
	itsPlotRangeDialog->BeginDialog();
}

/*******************************************************************************
 GetNewRange (private)

 ******************************************************************************/

void
JX2DPlotWidget::GetNewRange()
{
	assert( itsPlotRangeDialog != NULL );

	JFloat xmax, xmin, ymax, ymin;
	if (itsPlotRangeDialog->GetRangeValues(&xmax, &xmin, &ymax, &ymin))
		{
		SetRange(xmin, xmax, ymin, ymax);
		}
	else
		{
		ClearRange();
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

JBoolean
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
	assert (itsCurveOptionsDialog == NULL);

	JArray<JBoolean> hasXErrors, hasYErrors, isFunction, isScatter;

	const JSize count = GetCurveCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JPlotDataBase& curve = GetCurve(i);
		hasXErrors.AppendElement(curve.HasXErrors());
		hasYErrors.AppendElement(curve.HasYErrors());
		isFunction.AppendElement(curve.IsFunction());
		isScatter.AppendElement(JI2B(curve.GetType() == JPlotDataBase::kScatterPlot));
		}

	itsCurveOptionsDialog =
		new JX2DCurveOptionsDialog(GetWindow()->GetDirector(), GetCurveInfoArray(),
									hasXErrors, hasYErrors, isFunction, isScatter, index);
	assert (itsCurveOptionsDialog != NULL);
	ListenTo(itsCurveOptionsDialog);
	itsCurveOptionsDialog->BeginDialog();
}

/*******************************************************************************
 GetNewCurveOptions

 ******************************************************************************/

void
JX2DPlotWidget::GetNewCurveOptions()
{
	assert( itsCurveOptionsDialog != NULL );
	SetCurveInfoArray(itsCurveOptionsDialog->GetCurveInfoArray());
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

	itsCursorMenu->SetItemEnable(kMarkCursorCmd, CursorIsSelected());
	itsCursorMenu->SetItemEnable(kMarkAllVisCursorsCmd,
								 JI2B(XCursorVisible() || YCursorVisible()));
	itsCursorMenu->SetItemEnable(kRemoveAllMarksCmd,
								JI2B(GetXMarkCount() > 0 || GetYMarkCount() > 0));
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
		str += JString(i, JString::kBase10);
		itsMarkMenu->AppendItem(str);
		}

	if (xCount > 0 && yCount > 0)
		{
		itsMarkMenu->ShowSeparatorAfter(xCount);
		}

	for (i=1; i<=yCount; i++)
		{
		JString str("y");
		str += JString(i, JString::kBase10);
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

	const JPlotDataBase& curve = GetCurve(itsCurveOptionsIndex);

	// visibility

	if (CurveIsVisible(itsCurveOptionsIndex))
		{
		itsCurveOptionsMenu->CheckItem(kToggleCurveVisibleCmd);
		}

	// style

	JIndex style;
	if (curve.GetType() != JPlotDataBase::kScatterPlot)
		{
		style = (SymbolsAreVisible(itsCurveOptionsIndex) ? kSymbolsCmd : kPointsCmd);
		itsCurveOptionsMenu->EnableItem(kPointsCmd);
		itsCurveOptionsMenu->EnableItem(kSymbolsCmd);
		itsCurveOptionsMenu->DisableItem(kLinesCmd);
		itsCurveOptionsMenu->DisableItem(kLinesSymbolsCmd);
		}
	else if (curve.IsFunction())
		{
		style = kLinesCmd;
		itsCurveOptionsMenu->DisableItem(kPointsCmd);
		itsCurveOptionsMenu->DisableItem(kSymbolsCmd);
		itsCurveOptionsMenu->EnableItem(kLinesCmd);
		itsCurveOptionsMenu->DisableItem(kLinesSymbolsCmd);
		}
	else
		{
		const JBoolean lines   = LinesAreVisible(itsCurveOptionsIndex);
		const JBoolean symbols = SymbolsAreVisible(itsCurveOptionsIndex);
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

	itsCurveOptionsMenu->SetItemEnable(kXErrorsCmd, curve.HasXErrors());
	if (curve.HasXErrors() && XErrorsAreVisible(itsCurveOptionsIndex))
		{
		itsCurveOptionsMenu->CheckItem(kXErrorsCmd);
		}

	itsCurveOptionsMenu->SetItemEnable(kYErrorsCmd, curve.HasYErrors());
	if (curve.HasYErrors() && YErrorsAreVisible(itsCurveOptionsIndex))
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
		SetCurveStyle(itsCurveOptionsIndex, kJFalse, kJFalse);
		}
	else if (index == kSymbolsCmd)
		{
		SetCurveStyle(itsCurveOptionsIndex, kJFalse, kJTrue);
		}
	else if (index == kLinesCmd)
		{
		SetCurveStyle(itsCurveOptionsIndex, kJTrue, kJFalse);
		}
	else if (index == kLinesSymbolsCmd)
		{
		SetCurveStyle(itsCurveOptionsIndex, kJTrue, kJTrue);
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
	if (itsPSPrinter != NULL)
		{
		StopListening(itsPSPrinter);
		}
	itsPSPrinter = p;
}

/******************************************************************************
 HandlePSPageSetup

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JX2DPlotWidget::HandlePSPageSetup()
{
	assert( itsPSPrinter != NULL );

	itsPSPrinter->BeginUserPageSetup();
}

/******************************************************************************
 PrintPS

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JX2DPlotWidget::PrintPS()
{
	assert( itsPSPrinter != NULL );

	itsPSPrinter->SetFileName(itsPSPrintName);
	itsPSPrinter->BeginUserPrintSetup();
	ListenTo(itsPSPrinter);
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

	PostScript.  Returns kJFalse if printing was cancelled.

 ******************************************************************************/

JBoolean
JX2DPlotWidget::PrintMarks
	(
	JPagePrinter&	p,
	const JBoolean	putOnSamePage,
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
	if (itsEPSPrinter != NULL)
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
	assert( itsEPSPrinter != NULL );

	itsPrintEPSPlotFlag = kJTrue;
	itsEPSPrinter->SetOutputFileName(itsEPSPlotName);
	itsEPSPrinter->UsePlotSetup(kJTrue);
	if (!itsEPSPlotBounds.IsEmpty())
		{
		itsEPSPrinter->SetPlotBounds(itsEPSPlotBounds);
		}
	itsEPSPrinter->BeginUserPrintSetup();
	ListenTo(itsEPSPrinter);
}

/******************************************************************************
 PrintMarksEPS

	You must call SetEPSPrinter() before using this routine.

 ******************************************************************************/

void
JX2DPlotWidget::PrintMarksEPS()
{
	assert( itsEPSPrinter != NULL );

	itsPrintEPSPlotFlag = kJFalse;
	itsEPSPrinter->SetOutputFileName(itsEPSMarksName);
	itsEPSPrinter->UsePlotSetup(kJFalse);
	itsEPSPrinter->BeginUserPrintSetup();
	ListenTo(itsEPSPrinter);
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
	istream& input
	)
{
	JFileVersion vers = 0;
	input >> ws;
	const JCharacter c1 = input.peek();
	if ('2' <= c1 && c1 <= '9')			// version 1 => leave it for J2DPlotWidget
		{
		input >> vers;
		assert( vers <= kCurrentSetupVersion );
		}

	J2DPlotWidget::PWReadSetup(input);

	itsMarkDir->ReadSetup(input);
	JBoolean active;
	input >> active;
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
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	output << ' ';
	J2DPlotWidget::PWWriteSetup(output);

	output << ' ';
	itsMarkDir->WriteSetup(output);
	output << ' ' << itsMarkDir->IsActive();

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
