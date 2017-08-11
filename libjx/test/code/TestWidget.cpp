/******************************************************************************
 TestWidget.cpp

	BASE CLASS = JXScrollableWidget

	Written by John Lindal.

 ******************************************************************************/

#include "TestWidget.h"
#include "ResizeWidgetDialog.h"

#include <JXDisplay.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXFileSelection.h>
#include <JXColormap.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXDragPainter.h>
#include <JXImagePainter.h>
#include <JXImageMask.h>
#include <JXWindowIcon.h>
#include <JXFontManager.h>
#include <JXCursor.h>
#include <jXGlobals.h>
#include <jXUtil.h>
#include <X11/cursorfont.h>

#include <JPagePrinter.h>
#include <JEPSPrinter.h>
#include <JRegex.h>
#include <JString.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jTime.h>
#include <jMath.h>
#include <jAssert.h>

// Actions menu

static const JCharacter* kActionsMenuTitleStr    = "Weird things to do";
static const JCharacter* kActionsMenuShortcutStr = "#D";
static const JCharacter* kActionsMenuStr =
	"    Change size"
	"%l| Fill %b %h f %k Ctrl-Shift-F"
	"  | Points %h p"
	"%l| Hide"
	"  | Deactivate"
	"%l| Hide quit"
	"  | Deactivate quit"
	"%l| Print selection targets"
	"  | Print old selection targets (t - 10 sec)"
	"%l| Advice %h a"
	"  | xlsfonts"
	"  | empty";

enum
{
	kChangeSizeCmd = 1,
	kToggleFillCmd,
	kPointMenuCmd,
	kShowHideCmd,
	kActDeactCmd,
	kShowHideQuitCmd,
	kActDeactQuitCmd,
	kPrintSelectionTargetsCmd,
	kPrintOldSelectionTargetsCmd,
	kAdviceMenuCmd,
	kXlsfontsMenuCmd,
	kEmptyMenuCmd
};

static const JCharacter* kShowStr = "Show";
static const JCharacter* kHideStr = "Hide";

static const JCharacter* kActivateStr   = "Activate";
static const JCharacter* kDeactivateStr = "Deactivate";

static const JCharacter* kRedStr   = "Red";
static const JCharacter* kGreenStr = "Green";

static const JCharacter* kShowQuitStr = "Show quit";
static const JCharacter* kHideQuitStr = "Hide quit";

static const JCharacter* kActivateQuitStr   = "Activate quit";
static const JCharacter* kDeactivateQuitStr = "Deactivate quit";

// Points menu

static const JCharacter* kPointMenuStr =
	"10 %r %h 1 | 20 %r %h 2 | 30 %r %h 3 | 40 %r %h 4 | 50 %r %h 5 | 60 %r %h 6";

// Advice menus

static const JCharacter* kAdviceMenuStr[] =
{
	"***|This is %h t", "***|not %h n", "***|a good way %h g",
	"***|to use %h u", "***|submenus!!! %h s"
};

const JSize kAdviceMenuCount      = sizeof(kAdviceMenuStr)/sizeof(JCharacter*);
const JIndex kAdviceBoldMenuIndex = 2;

// Secret popup menu

static const JCharacter* kSecretMenuStr =
	"Congratulations! | You found the secret menu."
	"%l| If you have to use such menus"
	"%l| Pick me! %k Ctrl-plus"
	"%l| ba x-a aha %h a | ba b %h b | ba x-a %h a | ba xa %h a";

const JIndex kSecretSubmenuIndex  = 3;
const JIndex kSecretMenuDialogCmd = 4;

static const JCharacter* kSecretSubmenuStr =
	"be sure to advertise them clearly elsewhere!";

// test xpm data

/* XPM */
static char * macapp_xpm[] = {
"16 16 5 1",
" 	c None",
".	c #5A5A59595A5A",
"X	c #FFFFFFFFFFFF",
"o	c #FFFFE7E79C9C",
"O	c #000000000000",
"                ",
"        .       ",
"       .X.      ",
"      .XXX.     ",
"     .XXXXX.    ",
"    .XXXX..X.   ",
"   .XXXX.oo.X.  ",
"    .XXOOOOoO   ",
"    O.XXX.oOOO  ",
"     O.XXXOOOO  ",
"      O.X.O OO  ",
"       O.O      ",
"        O       ",
"                ",
"                ",
"                "};

// home symbol

/* XPM */
static char * home_xpm[] = {
"16 16 2 1",
" 	c None",
"X	c black",
"           XX   ",
"      XXXX XX   ",
"     X    XXX   ",
"    X  XX  XX   ",
"   X  X  X  X   ",
"  X    XX    X  ",
" XX          XX ",
"X X  XX  XX  X X",
"  X  XX  XX  X  ",
"  X          X  ",
"  X XXX  XX  X  ",
"  X X X  XX  X  ",
"  X X X      X  ",
"  XXXXXXXXXXXX  ",
"                ",
"                "};

/******************************************************************************
 Constructor

 ******************************************************************************/

TestWidget::TestWidget
	(
	const JBoolean		isMaster,
	const JBoolean		isImage,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsRNG()
{
JIndex i;

	itsFillFlag       = kJFalse;
	itsRandPointCount = 10;
	itsResizeDialog   = NULL;

	SetBackColor(GetColormap()->GetDefaultBackColor());

	// cursors

	JXDisplay* display = GetDisplay();
	itsTrekCursor      = display->CreateBuiltInCursor("XC_trek",     XC_trek);
	itsGumbyCursor     = display->CreateBuiltInCursor("XC_gumby",    XC_gumby);
	itsBogosityCursor  = display->CreateBuiltInCursor("XC_bogosity", XC_bogosity);
	itsFleurCursor     = display->CreateBuiltInCursor("XC_fleur",    XC_fleur);
	SetDefaultCursor(itsTrekCursor);

	// menus

	itsActionsMenu = menuBar->AppendTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetTitleFontStyle(GetColormap()->GetWhiteColor());
	itsActionsMenu->SetShortcuts(kActionsMenuShortcutStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsPointMenu = jnew JXTextMenu(itsActionsMenu, kPointMenuCmd, menuBar);
	assert( itsPointMenu != NULL );
	itsPointMenu->SetMenuItems(kPointMenuStr);
	itsPointMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPointMenu);

	// This tests the JX response to an empty menu.
	JXTextMenu* emptyMenu = jnew JXTextMenu(itsActionsMenu, kEmptyMenuCmd, menuBar);
	assert( emptyMenu != NULL );

	JXMenu* prevMenu     = itsActionsMenu;
	JIndex prevMenuIndex = kAdviceMenuCmd;
	for (i=1; i<=kAdviceMenuCount; i++)
		{
		JXTextMenu* adviceMenu = jnew JXTextMenu(prevMenu, prevMenuIndex, menuBar);
		assert( adviceMenu != NULL );
		adviceMenu->SetMenuItems(kAdviceMenuStr[i-1]);
		adviceMenu->SetUpdateAction(JXMenu::kDisableNone);

		if (i == kAdviceBoldMenuIndex)
			{
			adviceMenu->SetItemFontStyle(2,
				JFontStyle(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor()));
			}

		prevMenu      = adviceMenu;
		prevMenuIndex = 2;
		}

	BuildXlsfontsMenu(itsActionsMenu, menuBar);

	// secret menus are a bad idea because the user can't find them!

	itsSecretMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsSecretMenu != NULL );
	itsSecretMenu->SetMenuItems(kSecretMenuStr);
	itsSecretMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsSecretMenu->SetToHiddenPopupMenu(kJTrue);		// will assert() otherwise
	itsSecretMenu->Hide();
	ListenTo(itsSecretMenu);

	itsSecretSubmenu = jnew JXTextMenu(itsSecretMenu, kSecretSubmenuIndex, this);
	assert( itsSecretSubmenu != NULL );
	itsSecretSubmenu->SetMenuItems(kSecretSubmenuStr);
	itsSecretSubmenu->SetUpdateAction(JXMenu::kDisableNone);
	// we don't ListenTo() it because it's only there for show

	// image from xpm

	itsXPMImage = jnew JXImage(GetDisplay(), JXPM(macapp_xpm));
	assert( itsXPMImage != NULL );

	// partial image from image

	itsPartialXPMImage = jnew JXImage(*itsXPMImage, JRect(5,5,14,16));
	assert( itsPartialXPMImage != NULL );

	// home symbol

	itsHomeImage = jnew JXImage(GetDisplay(), JXPM(home_xpm));
	assert( itsHomeImage != NULL );

	itsHomeRect = itsHomeImage->GetBounds();
	itsHomeRect.Shift(120, 10);

	// buffer contents of Widget in JXImage

	itsImageBuffer = NULL;
	if (isImage)
		{
		CreateImageBuffer();
		}

	// initial size

	SetBounds(400,400);

	// enclosed objects

	itsAnimButton = 
		jnew JXTextButton("Start", this, JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 37,175, 50,30);
	assert( itsAnimButton != NULL );
	itsAnimButton->SetShortcuts("#A");
	ListenTo(itsAnimButton);

	if (isMaster)
		{
		itsQuitButton = 
			jnew JXTextButton(JGetString("Quit::TestWidget"), this, JXWidget::kFixedRight, JXWidget::kFixedBottom,
							 x,y, 50,30);
		assert( itsQuitButton != NULL );

		JXColormap* colormap = GetColormap();
		itsQuitButton->CenterWithinEnclosure(kJTrue, kJTrue);
		itsQuitButton->SetFontStyle(JFontStyle(kJTrue, kJFalse, 0, kJFalse, colormap->GetRedColor()));
		itsQuitButton->SetNormalColor(colormap->GetCyanColor());
		itsQuitButton->SetPushedColor(colormap->GetBlueColor());

		ListenTo(itsQuitButton);
		}
	else
		{
		itsQuitButton = NULL;
		}

	ExpandToFitContent();

	// drops on iconfied window

	JXWindowIcon* windowIcon;
	const JBoolean hasIconWindow = GetWindow()->GetIconWidget(&windowIcon);
	assert( hasIconWindow );
	ListenTo(windowIcon);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestWidget::~TestWidget()
{
	jdelete itsXPMImage;
	jdelete itsPartialXPMImage;
	jdelete itsHomeImage;
	jdelete itsImageBuffer;
}

/*****************************************************************************
 Print

 ******************************************************************************/

void
TestWidget::Print
	(
	JPagePrinter& p
	)
{
	if (!p.OpenDocument())
		{
		return;
		}

	const JCoordinate headerHeight = p.JPainter::GetLineHeight();
	const JCoordinate footerHeight = JRound(1.5 * headerHeight);

	const JString dateStr = JGetTimeStamp();

	JBoolean cancelled = kJFalse;
	for (JIndex i=1; i<=3; i++)
		{
		if (!p.NewPage())
			{
			cancelled = kJTrue;
			break;
			}

		// draw the header

		JRect pageRect = p.GetPageRect();
		p.String(pageRect.left, pageRect.top, "testjx TestWidget");
		p.String(pageRect.left, pageRect.top, dateStr,
				 pageRect.width(), JPainter::kHAlignRight);
		p.LockHeader(headerHeight);

		// draw the footer

		pageRect = p.GetPageRect();
		const JString pageNumberStr = "Page " + JString(i);
		p.String(pageRect.left, pageRect.bottom - footerHeight, pageNumberStr,
				 pageRect.width(), JPainter::kHAlignCenter,
				 footerHeight, JPainter::kVAlignBottom);
		p.LockFooter(footerHeight);

		// draw the page

		DrawStuff(p);
		}

	if (!cancelled)
		{
		p.CloseDocument();
		}
}

/*****************************************************************************
 Print

 ******************************************************************************/

void
TestWidget::Print
	(
	JEPSPrinter& p
	)
{
	const JRect bounds = GetBounds();

	if (p.WantsPreview())
		{
		JPainter& p1 = p.GetPreviewPainter(bounds);
		DrawStuff(p1);
		}

	if (p.OpenDocument(bounds))
		{
		DrawStuff(p);
		p.CloseDocument();
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
TestWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsImageBuffer != NULL)
		{
		p.JPainter::Image(*itsImageBuffer, itsImageBuffer->GetBounds(), 0,0);
		}
	else
		{
		DrawStuff(p);
		}
}

void
TestWidget::DrawStuff
	(
	JPainter& p
	)
{
JIndex i;

	JXColormap* colormap = GetColormap();

	p.SetPenColor(colormap->GetGreenColor());
	JRect ellipseRect(100,50,150,300);
	p.Ellipse(ellipseRect);

	p.SetPenColor(colormap->GetBlackColor());

	if (itsFillFlag)
		{
		p.SetFilling(kJTrue);
		}

	JRect ap = GetAperture();
	p.Line(ap.topLeft(), ap.bottomRight());
	p.Line(ap.topRight(), ap.bottomLeft());

	p.SetLineWidth(2);
	p.SetFontName("Times");
	p.SetFontSize(18);

	p.Image(*itsHomeImage, itsHomeImage->GetBounds(), itsHomeRect);

	its2Rect = JRect(150, 5, 200, 30);
	p.SetPenColor(colormap->GetRedColor());
	p.Rect(its2Rect);
	p.SetFontStyle(colormap->GetRedColor());
	p.String(its2Rect.topLeft(), "2",
			 its2Rect.width(),  JPainter::kHAlignCenter,
			 its2Rect.height(), JPainter::kVAlignCenter);

	its3Rect = JRect(10, 150, 40, 200);
	p.SetPenColor(colormap->GetBlueColor());
	p.Rect(its3Rect);
	p.SetFontStyle(colormap->GetBlueColor());
	p.String(its3Rect.topLeft(), "3",
			 its3Rect.width(),  JPainter::kHAlignCenter,
			 its3Rect.height(), JPainter::kVAlignCenter);

	p.SetLineWidth(1);
	p.SetFont(GetFontManager()->GetDefaultFont());

	p.ShiftOrigin(10,10);

	p.Point(0,0);
	for (i=1; i<=itsRandPointCount; i++)
		{
		p.Point(itsRNG.UniformLong(0,200), itsRNG.UniformLong(0,200));
		}

	p.SetPenColor(colormap->GetRedColor());
	p.Line(10,0, 0,10);
	p.SetPenColor(colormap->GetGreenColor());
	p.LineTo(10,20);
	p.SetPenColor(colormap->GetBlueColor());
	p.LineTo(0,30);

	p.ShiftOrigin(2,0);

	JPoint textPt(40,30);
	p.String(  0.0, textPt, "Hello");
	p.String( 90.0, textPt, "Hello");
	p.String(180.0, textPt, "Hello");
	p.String(270.0, textPt, "Hello");

	p.ShiftOrigin(-2, 0);

	p.SetPenColor(colormap->GetBlueColor());
	JRect r(70, 290, 150, 390);
	p.Rect(r);
/*
	for (JCoordinate y=70; y<150; y++)
		{
		p.SetPenColor(colormap->GetGrayColor(y-50));
		p.Line(290,y, 390,y);
		}

	for (JCoordinate x=290; x<390; x++)
		{
		p.SetPenColor(colormap->GetGrayColor(x-290));
		p.Line(x,70, x,150);
		}

	p.SetLineWidth(2);
	for (JCoordinate y=70; y<150; y+=2)
		{
		p.SetPenColor(colormap->GetGrayColor(y%4 ? 40 : 60));
		p.Line(290,y, 390,y);
		}
	p.SetLineWidth(1);

	p.SetLineWidth(2);
	for (JCoordinate x=290; x<390; x+=2)
		{
		p.SetPenColor(colormap->GetGrayColor(x%4 ? 40 : 60));
		p.Line(x,70, x,150);
		}
	p.SetLineWidth(1);
*/
	p.String(  0.0, r, "Hello", JPainter::kHAlignCenter, JPainter::kVAlignCenter);
	p.String( 90.0, r, "Hello", JPainter::kHAlignCenter, JPainter::kVAlignCenter);
	p.String(180.0, r, "Hello", JPainter::kHAlignCenter, JPainter::kVAlignCenter);
	p.String(270.0, r, "Hello", JPainter::kHAlignCenter, JPainter::kVAlignCenter);

	p.String(  0.0, r, "Hello", JPainter::kHAlignRight, JPainter::kVAlignBottom);
	p.String( 90.0, r, "Hello", JPainter::kHAlignRight, JPainter::kVAlignBottom);
	p.String(180.0, r, "Hello", JPainter::kHAlignRight, JPainter::kVAlignBottom);
	p.String(270.0, r, "Hello", JPainter::kHAlignRight, JPainter::kVAlignBottom);

	p.SetPenColor(colormap->GetBlueColor());
	p.Rect(200, 10, 100, 50);
	p.String(200, 10, "Hello", 100, JPainter::kHAlignLeft);
	p.String(200, 10+p.GetLineHeight(), "Hello", 100, JPainter::kHAlignCenter);
	p.String(200, 10+2*p.GetLineHeight(), "Hello", 100, JPainter::kHAlignRight);

	p.SetPenColor(colormap->GetDarkGreenColor());
	p.SetFilling(kJTrue);
	p.Rect(290, 160, 100, 80);
	p.SetFilling(kJFalse);
/*
	for (JCoordinate y=160; y<240; y++)
		{
		p.SetPenColor(colormap->GetGrayColor(y-140));
		p.Line(290,y, 390,y);
		}

	for (JCoordinate x=290; x<390; x++)
		{
		p.SetPenColor(colormap->GetGrayColor(x-290));
		p.Line(x,160, x,240);
		}

	p.SetLineWidth(2);
	for (JCoordinate y=160; y<240; y+=2)
		{
		p.SetPenColor(colormap->GetGrayColor(y%4 ? 40 : 60));
		p.Line(290,y, 390,y);
		}
	p.SetLineWidth(1);

	p.SetLineWidth(2);
	for (JCoordinate x=290; x<390; x+=2)
		{
		p.SetPenColor(colormap->GetGrayColor(x%4 ? 40 : 60));
		p.Line(x,160, x,240);
		}
	p.SetLineWidth(1);
*/
	textPt.Set(340, 200);
	p.SetFontName("Times");
	p.SetFontStyle(colormap->GetBlueColor());
	p.String(  0.0, textPt, "Hello");
	p.String( 90.0, textPt, "Hello");
	p.SetFontStyle(colormap->GetYellowColor());
	p.String(180.0, textPt, "Hello");
	p.String(270.0, textPt, "Hello");

	p.SetPenColor(colormap->GetYellowColor());
	r.Set(0,11,80,91);
	p.Rect(r);
	r.Shrink(1,1);
	p.SetPenColor(colormap->GetBlueColor());
	p.Ellipse(r);
	r.Shrink(1,1);
	p.SetPenColor(colormap->GetRedColor());
	p.Arc(r, 270.0-45.0, -270.0);

	JPolygon poly;
	poly.AppendElement(JPoint(0,85));
	poly.AppendElement(JPoint(10,85));
	poly.AppendElement(JPoint(5,95));
	p.Polygon(poly);

	p.Line(0,100, 2,98);
	p.LineTo(4,100);
	p.LineTo(2,102);
	p.LineTo(0,100);

	poly.SetElement(1, JPoint(0,5));
	poly.SetElement(2, JPoint(2,0));
	poly.SetElement(3, JPoint(4,5));
	p.Polygon(2,105, poly);

	// test filling rule

	p.SetPenColor(colormap->GetRedColor());
	p.SetFilling(kJTrue);

	JPolygon fillRulePoly;
	fillRulePoly.AppendElement(JPoint(175,45));
	fillRulePoly.AppendElement(JPoint(165,65));
	fillRulePoly.AppendElement(JPoint(190,50));
	fillRulePoly.AppendElement(JPoint(160,50));
	fillRulePoly.AppendElement(JPoint(185,65));
	p.Polygon(fillRulePoly);

	p.SetFilling(kJFalse);

	// dashed lines

	p.DrawDashedLines(kJTrue);

	p.SetPenColor(colormap->GetBlackColor());

	JArray<JSize> dashList;			// pixel rulers
	dashList.AppendElement(1);
	dashList.AppendElement(1);
	p.SetDashList(dashList);
	p.Line(100,110, 200,110);
	p.Line(100,114, 200,114);
	p.Line(100,118, 200,118);

	dashList.SetElement(1, 2);		// simple pattern
	dashList.SetElement(2, 3);
	p.SetDashList(dashList);
	p.Line(100,112, 200,112);

	p.SetFontStyle(JFontStyle(kJFalse, kJFalse, 1, kJFalse));
	p.String(130,155, "underline without dashes");

	p.SetDashList(dashList, 3);		// test offset
	p.Line(100,116, 200,116);

	dashList.SetElement(1, 1);		// example with odd # of values from X manuals
	dashList.SetElement(2, 2);
	dashList.AppendElement(3);
	p.SetDashList(dashList);
	p.Line(100,120, 200,120);

	dashList.SetElement(1, 5);		// dash-dot pattern
	dashList.SetElement(2, 2);
	dashList.SetElement(3, 1);
	dashList.AppendElement(2);
	p.SetDashList(dashList);
	p.Line(100,122, 200,122);

	p.Ellipse(210,110, 20,20);
	p.DrawDashedLines(kJFalse);
	p.Ellipse(213,113, 14,14);
	p.Rect(207,107, 26,26);
	p.SetPenColor(colormap->GetYellowColor());
	p.DrawDashedLines(kJTrue);
	p.Ellipse(213,113, 14,14);

	// icons

	p.Image(*itsXPMImage, itsXPMImage->GetBounds(), 33,110);
	p.Image(*itsPartialXPMImage, itsXPMImage->GetBounds(), 50,121);
}

/******************************************************************************
 BoundsResized (virtual protected)

	Redo itsImageBuffer to match our new size.

 ******************************************************************************/

void
TestWidget::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::BoundsResized(dw,dh);

	if (itsImageBuffer != NULL && (dw != 0 || dh != 0))
		{
		CreateImageBuffer();
		}
}

/******************************************************************************
 CreateImageBuffer (private)

	Note that this is a bad idea in general because it can use far more
	memory than buffered drawing:  pixels outside the aperture are stored.
	If the Widget redraws -really- slowly, however, one must decide for
	oneself how to trade off between time and memory.

	Note also that one only gets the speed increase if the image has
	remote storage.  With local storage, things slow down dramatically because
	the entire image has to be sent across the network every time it is
	drawn.

 ******************************************************************************/

void
TestWidget::CreateImageBuffer()
{
	// clear itsImageBuffer so Draw() will work correctly

	jdelete itsImageBuffer;
	itsImageBuffer = NULL;

	// create image

	const JRect bounds = GetBounds();
	JXImage* imageBuffer =
		jnew JXImage(GetDisplay(), bounds.width(), bounds.height());
	assert( imageBuffer != NULL );
	imageBuffer->SetDefaultState(JXImage::kRemoteStorage);

	// draw to image

	JXImagePainter* p = imageBuffer->CreatePainter();
	Draw(*p, GetBounds());
	jdelete p;

	// save object

	itsImageBuffer = imageBuffer;
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
TestWidget::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsHomeRect.Contains(pt))
		{
		DisplayCursor(kJXDefaultCursor);
		}
	else if (its2Rect.Contains(pt))
		{
		DisplayCursor(itsGumbyCursor);
		}
	else if (its3Rect.Contains(pt))
		{
		DisplayCursor(itsBogosityCursor);
		}
	else if (modifiers.meta())
		{
		DisplayCursor(itsFleurCursor);
		}
	else
		{
		JXScrollableWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
TestWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton && clickCount == 1 && itsHomeRect.Contains(pt))
		{
		JString dir;
		if (JGetHomeDirectory(&dir))
			{
			JPtrArray<JString> list(JPtrArrayT::kForgetAll);
			list.Append(&dir);

			// Normally, you should use the other constructor and then
			// override JXWidget::GetSelectionData().

			JXFileSelection* data = jnew JXFileSelection(GetDisplay(), list);
			assert( data != NULL );

			BeginDND(pt, buttonStates, modifiers, data);
			}
		}
	else if (button == kJXLeftButton && clickCount == 1)
		{
		JPainter* p = CreateDragInsidePainter();
		p->Rect(JRect(pt, pt));
		}
	else if (button == kJXMiddleButton && clickCount == 1)
		{
		itsAnimButton->Place(pt.x, pt.y);
		}
	else if (button == kJXRightButton && clickCount == 1 && !modifiers.meta())
		{
		JRect r = itsAnimButton->GetFrame();
		if (pt.x > r.left && pt.y > r.top)
			{
			itsAnimButton->SetSize(pt.x-r.left, pt.y-r.top);
			}
		}
	else if (button == kJXRightButton && clickCount == 1 && modifiers.meta())
		{
		if (itsSecretMenu->PopUp(this, pt, buttonStates, modifiers))
			{
			return;
			}
		else
			{
			(JGetUserNotification())->ReportError("Unable to open secret menu!");
			}
		}
	else if (ScrollForWheel(button, modifiers))
		{
		// work has been done
		}
	else if ((clickCount == 2 && its2Rect.Contains(pt)) ||
			 (clickCount == 3 && its3Rect.Contains(pt)))
		{
		GetNewSize();
		}

	itsStartPt = itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
TestWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean scrolled = ScrollForDrag(pt);

	JPainter* p = NULL;
	if (buttonStates.left() && pt != itsPrevPt && GetDragPainter(&p))	// no painter for multiple click
		{
		if (!scrolled)
			{
			p->Rect(JRect(itsStartPt, itsPrevPt));
			}
		p->Rect(JRect(itsStartPt, pt));
		}

	if (buttonStates.middle() && pt != itsPrevPt)
		{
		itsAnimButton->Place(pt.x, pt.y);
		}
	else if (buttonStates.right() && pt != itsPrevPt)
		{
		JRect r = itsAnimButton->GetFrame();
		if (pt.x > r.left && pt.y > r.top)
			{
			itsAnimButton->SetSize(pt.x-r.left, pt.y-r.top);
			}
		}

	itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
TestWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPainter* p = NULL;
	if (button == kJXLeftButton && GetDragPainter(&p))	// no painter for multiple click
		{
		p->Rect(JRect(itsStartPt, itsPrevPt));
		DeleteDragPainter();
		}
}

/******************************************************************************
 HitSamePart

 ******************************************************************************/

JBoolean
TestWidget::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	return JConvertToBoolean(
			(its2Rect.Contains(pt1) && its2Rect.Contains(pt2)) ||
			(its3Rect.Contains(pt1) && its3Rect.Contains(pt2)));
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
TestWidget::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	We accept all copy drops so we can print out the data types.

 ******************************************************************************/

JBoolean
TestWidget::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	JXDNDManager* dndMgr = GetDNDManager();
	JXDisplay* display   = GetDisplay();

	JBoolean hasURIList = kJFalse;

	std::cout << std::endl;
	std::cout << "Data types available from DND source:" << std::endl;
	std::cout << std::endl;

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom type = typeList.GetElement(i);
		std::cout << XGetAtomName(*display, type) << std::endl;

		if (type == GetSelectionManager()->GetURLXAtom())
			{
			hasURIList = kJTrue;
			}
		}

	if (hasURIList)
		{
		if (its2Rect.Contains(pt) || its3Rect.Contains(pt))
			{
			std::cout << std::endl;
			std::cout << "Accepting the drop of type text/uri-list" << std::endl;
			std::cout << std::endl;

			*action = dndMgr->GetDNDActionPrivateXAtom();
			return kJTrue;
			}
		else
			{
			return kJFalse;
			}
		}
	else if (*action == dndMgr->GetDNDActionCopyXAtom())
		{
		std::cout << std::endl;
		std::cout << "Accepting the drop" << std::endl;
		std::cout << std::endl;

		PrintSelectionText(dndMgr->GetDNDSelectionName(), time,
						   GetSelectionManager()->GetMimePlainTextXAtom());
		return kJTrue;
		}
	else
		{
		std::cout << std::endl;
		std::cout << "Not accepting the drop because the action isn't copy" << std::endl;
		std::cout << "Action: " << XGetAtomName(*display, *action) << std::endl;
		std::cout << std::endl;

		PrintSelectionText(dndMgr->GetDNDSelectionName(), time,
						   GetSelectionManager()->GetMimePlainTextXAtom());

		return kJFalse;
		}
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

 ******************************************************************************/

void
TestWidget::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXDisplay* display         = GetDisplay();
	JXSelectionManager* selMgr = GetSelectionManager();

	std::cout << std::endl;
	std::cout << "Data types available from DND source:" << std::endl;
	std::cout << std::endl;

//	(JGetUserNotification())->DisplayMessage("testing");

	Atom textType = None;
	JBoolean url  = kJFalse;
	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom type = typeList.GetElement(i);
		std::cout << XGetAtomName(*display, type) << std::endl;
		if (type == selMgr->GetMimePlainTextXAtom())
			{
			textType = type;
			}
		else if (type == selMgr->GetURLXAtom())
			{
			url = kJTrue;
			}
		}
	std::cout << std::endl;

	if (textType != None)
		{
		PrintSelectionText(GetDNDManager()->GetDNDSelectionName(), time, textType);
		}

	if (url)
		{
		PrintFileNames(GetDNDManager()->GetDNDSelectionName(), time);
		}
}

/******************************************************************************
 PrintSelectionTargets (private)

 ******************************************************************************/

void
TestWidget::PrintSelectionTargets
	(
	const Time time
	)
{
	JXDisplay* display         = GetDisplay();
	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();

	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(kJXClipboardName, time, &typeList))
		{
		std::cout << std::endl;
		std::cout << "Data types available from the clipboard:" << std::endl;
		std::cout << std::endl;

		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
			{
			const Atom type = typeList.GetElement(i);
			std::cout << XGetAtomName(*display, type) << std::endl;
			}

		for (JIndex i=1; i<=typeCount; i++)
			{
			const Atom type = typeList.GetElement(i);
			if (type == XA_STRING ||
				type == selMgr->GetUtf8StringXAtom() ||
				type == selMgr->GetMimePlainTextXAtom())
				{
				std::cout << std::endl;
				PrintSelectionText(kJXClipboardName, time, type);
				}
			}
		}
	else
		{
		std::cout << std::endl;
		std::cout << "Unable to access the clipboard." << std::endl;
		std::cout << std::endl;
		}
}

/******************************************************************************
 PrintSelectionText (private)

 ******************************************************************************/

void
TestWidget::PrintSelectionText
	(
	const Atom selectionName,
	const Time time,
	const Atom type
	)
	const
{
	JXDisplay* display         = GetDisplay();
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(selectionName, time, type,
						&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == XA_STRING ||
			returnType == selMgr->GetUtf8StringXAtom() ||
			returnType == selMgr->GetMimePlainTextXAtom())
			{
			std::cout << "Data is available as " << XGetAtomName(*display, type) << ":" << std::endl << std::endl;

			std::cout.write((char*) data, dataLength);
			std::cout << std::endl << std::endl;
			}
		else
			{
			std::cout << "Data has unrecognized return type:  ";
			std::cout << XGetAtomName(*(GetDisplay()), returnType) << std::endl;
			std::cout << "Trying to print it anyway:" << std::endl << std::endl;

			std::cout.write((char*) data, dataLength);
			std::cout << std::endl << std::endl;
			}

		selMgr->DeleteData(&data, delMethod);
		}
	else
		{
		std::cout << "Data could not be retrieved as " << XGetAtomName(*display, type) << "." << std::endl << std::endl;
		}
}

/******************************************************************************
 PrintFileNames (private)

 ******************************************************************************/

void
TestWidget::PrintFileNames
	(
	const Atom selectionName,
	const Time time
	)
	const
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(selectionName, time, selMgr->GetURLXAtom(),
						&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == selMgr->GetURLXAtom())
			{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);

			const JSize fileCount = fileNameList.GetElementCount();
			if (fileCount > 0)
				{
				std::cout << "File/directory names:" << std::endl << std::endl;
				for (JIndex i=1; i<=fileCount; i++)
					{
					std::cout << *(fileNameList.NthElement(i)) << std::endl;
					}
				std::cout << std::endl << std::endl;
				}

			const JSize urlCount = urlList.GetElementCount();
			if (urlCount > 0)
				{
				std::cout << "Unconvertable URLs:" << std::endl << std::endl;
				for (JIndex i=1; i<=urlCount; i++)
					{
					std::cout << *(urlList.NthElement(i)) << std::endl;
					}
				std::cout << std::endl << std::endl;
				}

			JXReportUnreachableHosts(urlList);
			}

		selMgr->DeleteData(&data, delMethod);
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
TestWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
/*
	std::cout << std::endl;
	std::cout << "Shift pressed  : " << modifiers.shift()   << std::endl;
	std::cout << "Control pressed: " << modifiers.control() << std::endl;
	std::cout << "Meta pressed   : " << modifiers.meta()    << std::endl;
*/
	JXScrollableWidget::HandleKeyPress(key, modifiers);
}

/******************************************************************************
 HandleClientMessage (virtual protected)

	This is for testing purposes.  It prints out every ClientMessage
	that it receives.

 ******************************************************************************/

JBoolean
TestWidget::HandleClientMessage
	(
	const XClientMessageEvent& clientMessage
	)
{
	std::cout << std::endl;
	std::cout << "Window id: " << clientMessage.window << std::endl;
	std::cout << "Message:   " << clientMessage.message_type << "  (";
	std::cout << XGetAtomName(*(GetDisplay()), clientMessage.message_type);
	std::cout << ')' << std::endl;
	std::cout << "Format:    " << clientMessage.format << " bit data" << std::endl;

	std::cout << "Data:" << std::endl;
	if (clientMessage.format == 8)
		{
		for (JIndex i=0; i<20; i++)
			{
			std::cout << clientMessage.data.b[i] << ' ';
			}
		}
	else if (clientMessage.format == 16)
		{
		for (JIndex i=0; i<10; i++)
			{
			std::cout << clientMessage.data.s[i] << ' ';
			}
		}
	else
		{
		for (JIndex i=0; i<5; i++)
			{
			std::cout << clientMessage.data.l[i] << ' ';
			}
		}
	std::cout << std::endl << std::endl;

	return JXScrollableWidget::HandleClientMessage(clientMessage);
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
TestWidget::UpdateActionsMenu()
{
	if (itsFillFlag)
		{
		itsActionsMenu->CheckItem(kToggleFillCmd);
		}

	itsActionsMenu->SetItemEnable(kShowHideQuitCmd,
								  JI2B(itsQuitButton != NULL));
	itsActionsMenu->SetItemEnable(kActDeactQuitCmd,
								  JI2B(itsQuitButton != NULL));
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
TestWidget::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kChangeSizeCmd)
		{
		GetNewSize();
		}
	else if (index == kToggleFillCmd)
		{
		itsFillFlag = !itsFillFlag;
		Refresh();
		}

	else if (index == kShowHideCmd && IsVisible())
		{
		Hide();
		itsActionsMenu->SetItemText(kShowHideCmd, kShowStr);
		}
	else if (index == kShowHideCmd)
		{
		Show();
		itsActionsMenu->SetItemText(kShowHideCmd, kHideStr);
		}

	else if (index == kActDeactCmd && IsActive())
		{
		Deactivate();
		itsActionsMenu->SetItemText(kActDeactCmd, kActivateStr);
		}
	else if (index == kActDeactCmd)
		{
		Activate();
		itsActionsMenu->SetItemText(kActDeactCmd, kDeactivateStr);
		}

	else if (index == kShowHideQuitCmd && itsQuitButton != NULL &&
			 itsQuitButton->WouldBeVisible())
		{
		itsQuitButton->Hide();
		itsActionsMenu->SetItemText(kShowHideQuitCmd, kShowQuitStr);
		}
	else if (index == kShowHideQuitCmd && itsQuitButton != NULL)
		{
		itsQuitButton->Show();
		itsActionsMenu->SetItemText(kShowHideQuitCmd, kHideQuitStr);
		}

	else if (index == kActDeactQuitCmd && itsQuitButton != NULL &&
			 itsQuitButton->WouldBeActive())
		{
		itsQuitButton->Deactivate();
		itsActionsMenu->SetItemText(kActDeactQuitCmd, kActivateQuitStr);
		}
	else if (index == kActDeactQuitCmd && itsQuitButton != NULL)
		{
		itsQuitButton->Activate();
		itsActionsMenu->SetItemText(kActDeactQuitCmd, kDeactivateQuitStr);
		}

	else if (index == kPrintSelectionTargetsCmd)
		{
		PrintSelectionTargets(CurrentTime);
		}
	else if (index == kPrintOldSelectionTargetsCmd)
		{
		PrintSelectionTargets(GetDisplay()->GetLastEventTime() - 10000);
		}
}

/******************************************************************************
 UpdatePointMenu (private)

 ******************************************************************************/

void
TestWidget::UpdatePointMenu()
{
	itsPointMenu->CheckItem(itsRandPointCount / 10);
}

/******************************************************************************
 HandlePointMenu (private)

 ******************************************************************************/

void
TestWidget::HandlePointMenu
	(
	const JIndex index
	)
{
	itsRandPointCount = 10 * index;
	Refresh();
}

/******************************************************************************
 GetNewSize

 ******************************************************************************/

void
TestWidget::GetNewSize()
{
	assert( itsResizeDialog == NULL );

	itsResizeDialog = jnew ResizeWidgetDialog(GetWindow()->GetDirector(), this);
	assert( itsResizeDialog != NULL );
	ListenTo(itsResizeDialog);
	itsResizeDialog->BeginDialog();
}

/******************************************************************************
 ChangeSize

 ******************************************************************************/

void
TestWidget::ChangeSize()
{
	assert( itsResizeDialog != NULL );

	JCoordinate w,h;
	itsResizeDialog->GetNewSize(&w, &h);
	SetBounds(w,h);
}

/******************************************************************************
 Receive (protected)

	Listen for menu selections and button presses.

 ******************************************************************************/

void
TestWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXWindow* window         = GetWindow();		// insure that it isn't const
	JXWindowIcon* windowIcon = NULL;

	if (sender == itsAnimButton && message.Is(JXButton::kPushed))
		{
		if (GetCursorAnimator() == NULL)
			{
			CreateCursorAnimator();
			itsAnimButton->SetShortcuts("^o^m");
			itsAnimButton->SetLabel("Stop");
			}
		else
			{
			RemoveCursorAnimator();
			itsAnimButton->SetLabel("Start");
			itsAnimButton->SetShortcuts("#a");
			}
		}
	else if (sender == itsQuitButton && message.Is(JXButton::kPushed))
		{
		(JXGetApplication())->Quit();
		}

	else if (window->GetIconWidget(&windowIcon) &&
			 sender == windowIcon && message.Is(JXWindowIcon::kHandleDrop))
		{
		const JXWindowIcon::HandleDrop* data =
			dynamic_cast<const JXWindowIcon::HandleDrop*>(&message);
		assert( data != NULL );
		HandleDNDDrop(JPoint(0,0), data->GetTypeList(), data->GetAction(),
					  data->GetTime(), data->GetSource());
		}

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsPointMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePointMenu();
		}
	else if (sender == itsPointMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePointMenu(selection->GetIndex());
		}

	else if (sender == itsSecretMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		if (selection->GetIndex() == kSecretMenuDialogCmd)
			{
			(JGetUserNotification())->DisplayMessage(
				"This message is generated by an item on the secret popup menu.");
			}
		}

	else if (sender == itsResizeDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			ChangeSize();
			}
		itsResizeDialog = NULL;
		}

	else
		{
		JXScrollableWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
TestWidget::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	JXWindowIcon* windowIcon = NULL;

	if (GetWindow()->GetIconWidget(&windowIcon) &&
		sender == windowIcon && message->Is(JXWindowIcon::kAcceptDrop))
		{
		JXWindowIcon::AcceptDrop* data =
			dynamic_cast<JXWindowIcon::AcceptDrop*>(message);
		assert( data != NULL );
		if (!data->WillAcceptDrop())
			{
			data->ShouldAcceptDrop(
				WillAcceptDrop(data->GetTypeList(), data->GetActionPtr(),
							   data->GetPoint(), data->GetTime(), data->GetSource()));
			}
		}

	else
		{
		JXScrollableWidget::ReceiveWithFeedback(sender, message);
		}
}

/******************************************************************************
 BuildXlsfontsMenu

 ******************************************************************************/

void
TestWidget::BuildXlsfontsMenu
	(
	JXMenu*			owner,
	JXContainer*	enclosure
	)
{
	JXTextMenu* menu = jnew JXTextMenu(owner, kXlsfontsMenuCmd, enclosure);
	assert( menu != NULL );
	menu->SetUpdateAction(JXMenu::kDisableNone);

	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	GetXFontManager()->GetXFontNames(JRegex("^-.*-(courier|helvetica)-.*$"),
									   &fontList);
	const JSize fontCount = fontList.GetElementCount();
	for (JIndex i=1; i<=fontCount; i++)
		{
		JString* fontName = fontList.NthElement(i);
		menu->AppendItem(*fontName);
		}

	menu->AppendItem("I wish Netscape's bookmark list would scroll like this!!!");
	const JSize count = menu->GetItemCount();
	menu->SetItemFontStyle(count, JFontStyle(kJTrue, kJFalse, 0, kJFalse,
											 GetColormap()->GetBlackColor()));
	if (count > 1)
		{
		menu->ShowSeparatorAfter(count-1);
		}
}
