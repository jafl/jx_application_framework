/******************************************************************************
 TestWidget.cpp

	BASE CLASS = JXScrollableWidget

	Written by John Lindal.

 ******************************************************************************/

#include "TestWidget.h"
#include "ResizeWidgetDialog.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXDNDManager.h>
#include <jx-af/jx/JXFileSelection.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/JXImagePainter.h>
#include <jx-af/jx/JXImageMask.h>
#include <jx-af/jx/JXWindowIcon.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXCursor.h>
#include <jx-af/jx/JXUrgentFunctionTask.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jx/jXUtil.h>
#include <X11/cursorfont.h>

#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/JEPSPrinter.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

#include "TestWidget-Actions.h"
#include "TestWidget-Points.h"
#include "TestWidget-Secret.h"

// Advice menus

static const JUtf8Byte* kAdviceMenuStr[] =
{
	"***|This is %h t", "***|not %h n", "***|a good way %h g",
	"***|to use %h u", "***|submenus!!! %h s"
};

const JSize kAdviceMenuCount      = sizeof(kAdviceMenuStr)/sizeof(JUtf8Byte*);
const JIndex kAdviceBoldMenuIndex = 2;

// Secret popup menu

const JIndex kSecretSubmenuIndex  = 3;
const JIndex kSecretMenuDialogCmd = 4;

static const JUtf8Byte* kSecretSubmenuStr =
	"be sure to advertise them clearly elsewhere!";

// test xpm data

/* XPM */
static const char * macapp_xpm[] = {
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
static const char * home_xpm[] = {
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
	const bool			isMaster,
	const bool			isImage,
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
	itsFillFlag(false),
	itsRandPointCount(10)
{
	SetBackColor(JColorManager::GetDefaultBackColor());

	// cursors

	JXDisplay* display = GetDisplay();
	itsTrekCursor      = display->CreateBuiltInCursor("XC_trek",     XC_trek);
	itsGumbyCursor     = display->CreateBuiltInCursor("XC_gumby",    XC_gumby);
	itsBogosityCursor  = display->CreateBuiltInCursor("XC_bogosity", XC_bogosity);
	itsFleurCursor     = display->CreateBuiltInCursor("XC_fleur",    XC_fleur);
	SetDefaultCursor(itsTrekCursor);

	// menus

	itsActionsMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::TestWidget"));
	itsActionsMenu->SetTitleFontStyle(JColorManager::GetWhiteColor());
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsActionsMenu->AttachHandlers(this,
		&TestWidget::UpdateActionsMenu,
		&TestWidget::HandleActionsMenu);
	ConfigureActionsMenu(itsActionsMenu);

	itsPointMenu = jnew JXTextMenu(itsActionsMenu, kPointMenuCmd, menuBar);
	itsPointMenu->SetMenuItems(kPointsMenuStr);
	itsPointMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPointMenu->AttachHandlers(this,
		&TestWidget::UpdatePointMenu,
		&TestWidget::HandlePointMenu);
	ConfigurePointsMenu(itsPointMenu);

	// This tests the JX response to an empty menu.
	jnew JXTextMenu(itsActionsMenu, kEmptyMenuCmd, menuBar);

	JXMenu* prevMenu     = itsActionsMenu;
	JIndex prevMenuIndex = kAdviceMenuCmd;
	for (JIndex i=1; i<=kAdviceMenuCount; i++)
	{
		JXTextMenu* adviceMenu = jnew JXTextMenu(prevMenu, prevMenuIndex, menuBar);
		adviceMenu->SetMenuItems(kAdviceMenuStr[i-1]);
		adviceMenu->SetUpdateAction(JXMenu::kDisableNone);

		if (i == kAdviceBoldMenuIndex)
		{
			adviceMenu->SetItemFontStyle(2,
				JFontStyle(true, false, 0, false, JColorManager::GetBlackColor()));
		}

		prevMenu      = adviceMenu;
		prevMenuIndex = 2;
	}

	BuildXlsfontsMenu(itsActionsMenu, menuBar);

	// secret menus are a bad idea because the user can't find them!

	itsSecretMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	itsSecretMenu->SetMenuItems(kSecretMenuStr);
	itsSecretMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsSecretMenu->SetToHiddenPopupMenu(true);		// will assert() otherwise
	itsSecretMenu->Hide();
	itsSecretMenu->AttachHandlers(this,
		std::bind(&TestWidget::UpdatePointMenu, this),
		[](const JIndex& i)
		{
			if (i == kSecretMenuDialogCmd)
			{
				JGetUserNotification()->DisplayMessage(
					JGetString("SecretMenuMessage::TestWidget"));
			}
		});
	ConfigureSecretMenu(itsSecretMenu);

	itsSecretSubmenu = jnew JXTextMenu(itsSecretMenu, kSecretSubmenuIndex, this);
	itsSecretSubmenu->SetMenuItems(kSecretSubmenuStr);
	itsSecretSubmenu->SetUpdateAction(JXMenu::kDisableNone);
	// we don't ListenTo() it because it's only there for show

	// image from xpm

	itsXPMImage = jnew JXImage(GetDisplay(), JXPM(macapp_xpm));

	// partial image from image

	itsPartialXPMImage = jnew JXImage(*itsXPMImage, JRect(5,5,14,16));

	// home symbol

	itsHomeImage = jnew JXImage(GetDisplay(), JXPM(home_xpm));
	itsHomeRect  = itsHomeImage->GetBounds();
	itsHomeRect.Shift(120, 10);

	// buffer contents of Widget in JXImage

	itsImageBuffer = nullptr;
	if (isImage)
	{
		CreateImageBuffer();
	}

	// initial size

	SetBounds(400,400);

	// enclosed objects

	itsAnimButton = 
		jnew JXTextButton(JGetString("AnimationButtonStartLabel::TestWidget"),
						  this, JXWidget::kFixedLeft, JXWidget::kFixedTop,
						  37,175, 50,30);
	itsAnimButton->SetShortcuts(JGetString("AnimationButtonStartShortcut::TestWidget"));

	ListenTo(itsAnimButton, std::function([this](const JXButton::Pushed&)
	{
		if (GetCursorAnimator() == nullptr)
		{
			CreateCursorAnimator();
			itsAnimButton->SetShortcuts(JGetString("AnimationButtonStopShorcut::TestWidget"));
			itsAnimButton->SetLabel(JGetString("AnimationButtonStopLabel::TestWidget"));
		}
		else
		{
			RemoveCursorAnimator();
			itsAnimButton->SetLabel(JGetString("AnimationButtonStartLabel::TestWidget"));
			itsAnimButton->SetShortcuts(JGetString("AnimationButtonStartShortcut::TestWidget"));
		}
	}));

	if (isMaster)
	{
		itsQuitButton = 
			jnew JXTextButton(JGetString("QuitButtonLabel::TestWidget"),
							  this, JXWidget::kFixedRight, JXWidget::kFixedBottom,
							  x,y, 50,30);

		itsQuitButton->CenterWithinEnclosure(true, true);
		itsQuitButton->SetFontStyle(JFontStyle(true, false, 0, false, JColorManager::GetRedColor()));
		itsQuitButton->SetNormalColor(JColorManager::GetCyanColor());
		itsQuitButton->SetPushedColor(JColorManager::GetBlueColor());

		ListenTo(itsQuitButton, std::function([](const JXButton::Pushed&)
		{
			JXGetApplication()->Quit();
		}));
	}
	else
	{
		itsQuitButton = nullptr;
	}

	ExpandToFitContent();

	// drops on iconfied window

	auto* task = jnew JXUrgentFunctionTask(this, [this]()
	{
		JXWindowIcon* windowIcon;
		const bool hasIconWindow = GetWindow()->GetIconWidget(&windowIcon);
		assert( hasIconWindow );
		ListenTo(windowIcon, std::function([this](const JXWindowIcon::HandleDrop& data)
		{
			HandleDNDDrop(JPoint(0,0), data.GetTypeList(), data.GetAction(),
						  data.GetTime(), data.GetSource());
		}));
	},
	"Drop on iconified window");
	task->Go();
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

	const JCoordinate headerHeight = p.GetLineHeight();
	const JCoordinate footerHeight = JRound(1.5 * headerHeight);

	const JString dateStr = JGetTimeStamp();

	bool cancelled = false;
	for (const JIndex i : { 1,2,3 })
	{
		if (!p.NewPage())
		{
			cancelled = true;
			break;
		}

		// draw the header

		JRect pageRect = p.GetPageRect();
		p.String(pageRect.left, pageRect.top, JGetString("PageHeader::TestWidget"));
		p.String(pageRect.left, pageRect.top, dateStr,
				 pageRect.width(), JPainter::HAlign::kRight);
		p.LockHeader(headerHeight);

		// draw the footer

		pageRect = p.GetPageRect();
		const JString pageNumberStr(i, 0);

		const JUtf8Byte* map[] =
		{
			"page", pageNumberStr.GetBytes()
		};
		p.String(pageRect.left, pageRect.bottom - footerHeight,
				 JGetString("PageFooter::TestWidget", map, sizeof(map)),
				 pageRect.width(), JPainter::HAlign::kCenter,
				 footerHeight, JPainter::VAlign::kBottom);
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
	if (itsImageBuffer != nullptr)
	{
		p.Image(*itsImageBuffer, itsImageBuffer->GetBounds(), 0,0);
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

	p.SetPenColor(JColorManager::GetGreenColor());
	JRect ellipseRect(100,50,150,300);
	p.Ellipse(ellipseRect);

	p.SetPenColor(JColorManager::GetBlackColor());

	if (itsFillFlag)
	{
		p.SetFilling(true);
	}

	JRect ap = GetAperture();
	p.Line(ap.topLeft(), ap.bottomRight());
	p.Line(ap.topRight(), ap.bottomLeft());

	const JString timesFontName("Times");

	p.SetLineWidth(2);
	p.SetFontName(timesFontName);
	p.SetFontSize(18);

	p.Image(*itsHomeImage, itsHomeImage->GetBounds(), itsHomeRect);

	its2Rect = JRect(150, 5, 200, 30);
	p.SetPenColor(JColorManager::GetRedColor());
	p.Rect(its2Rect);
	p.SetFontStyle(JColorManager::GetRedColor());
	p.String(its2Rect.topLeft(), "2",
			 its2Rect.width(),  JPainter::HAlign::kCenter,
			 its2Rect.height(), JPainter::VAlign::kCenter);

	its3Rect = JRect(10, 150, 40, 200);
	p.SetPenColor(JColorManager::GetBlueColor());
	p.Rect(its3Rect);
	p.SetFontStyle(JColorManager::GetBlueColor());
	p.String(its3Rect.topLeft(), "3",
			 its3Rect.width(),  JPainter::HAlign::kCenter,
			 its3Rect.height(), JPainter::VAlign::kCenter);

	p.SetLineWidth(1);
	p.SetFont(JFontManager::GetDefaultFont());

	p.ShiftOrigin(10,10);

	p.Point(0,0);
	for (i=1; i<=itsRandPointCount; i++)
	{
		p.Point(itsRNG.UniformLong(0,200), itsRNG.UniformLong(0,200));
	}

	p.SetPenColor(JColorManager::GetRedColor());
	p.Line(10,0, 0,10);
	p.SetPenColor(JColorManager::GetGreenColor());
	p.LineTo(10,20);
	p.SetPenColor(JColorManager::GetBlueColor());
	p.LineTo(0,30);

	p.ShiftOrigin(2,0);

	const JString& helloStr = JGetString("Hello::TestWidget");

	JPoint textPt(40,30);
	p.String(  0.0, textPt, helloStr);
	p.String( 90.0, textPt, helloStr);
	p.String(180.0, textPt, helloStr);
	p.String(270.0, textPt, helloStr);

	p.ShiftOrigin(-2, 0);

	p.SetPenColor(JColorManager::GetBlueColor());
	JRect r(70, 290, 150, 390);
	p.Rect(r);
/*
	for (JCoordinate y=70; y<150; y++)
	{
		p.SetPenColor(JColorManager::GetGrayColor(y-50));
		p.Line(290,y, 390,y);
	}

	for (JCoordinate x=290; x<390; x++)
	{
		p.SetPenColor(JColorManager::GetGrayColor(x-290));
		p.Line(x,70, x,150);
	}

	p.SetLineWidth(2);
	for (JCoordinate y=70; y<150; y+=2)
	{
		p.SetPenColor(JColorManager::GetGrayColor(y%4 ? 40 : 60));
		p.Line(290,y, 390,y);
	}
	p.SetLineWidth(1);

	p.SetLineWidth(2);
	for (JCoordinate x=290; x<390; x+=2)
	{
		p.SetPenColor(JColorManager::GetGrayColor(x%4 ? 40 : 60));
		p.Line(x,70, x,150);
	}
	p.SetLineWidth(1);
*/
	p.String(  0.0, r, helloStr, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	p.String( 90.0, r, helloStr, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	p.String(180.0, r, helloStr, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	p.String(270.0, r, helloStr, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);

	p.String(  0.0, r, helloStr, JPainter::HAlign::kRight, JPainter::VAlign::kBottom);
	p.String( 90.0, r, helloStr, JPainter::HAlign::kRight, JPainter::VAlign::kBottom);
	p.String(180.0, r, helloStr, JPainter::HAlign::kRight, JPainter::VAlign::kBottom);
	p.String(270.0, r, helloStr, JPainter::HAlign::kRight, JPainter::VAlign::kBottom);

	p.SetPenColor(JColorManager::GetBlueColor());
	p.Rect(200, 10, 100, 50);
	p.String(200, 10, helloStr, 100, JPainter::HAlign::kLeft);
	p.String(200, 10+p.GetLineHeight(), helloStr, 100, JPainter::HAlign::kCenter);
	p.String(200, 10+2*p.GetLineHeight(), helloStr, 100, JPainter::HAlign::kRight);

	p.SetPenColor(JColorManager::GetDarkGreenColor());
	p.SetFilling(true);
	p.Rect(290, 160, 100, 80);
	p.SetFilling(false);
/*
	for (JCoordinate y=160; y<240; y++)
	{
		p.SetPenColor(JColorManager::GetGrayColor(y-140));
		p.Line(290,y, 390,y);
	}

	for (JCoordinate x=290; x<390; x++)
	{
		p.SetPenColor(JColorManager::GetGrayColor(x-290));
		p.Line(x,160, x,240);
	}

	p.SetLineWidth(2);
	for (JCoordinate y=160; y<240; y+=2)
	{
		p.SetPenColor(JColorManager::GetGrayColor(y%4 ? 40 : 60));
		p.Line(290,y, 390,y);
	}
	p.SetLineWidth(1);

	p.SetLineWidth(2);
	for (JCoordinate x=290; x<390; x+=2)
	{
		p.SetPenColor(JColorManager::GetGrayColor(x%4 ? 40 : 60));
		p.Line(x,160, x,240);
	}
	p.SetLineWidth(1);
*/
	textPt.Set(340, 200);
	p.SetFontName(timesFontName);
	p.SetFontStyle(JColorManager::GetBlueColor());
	p.String(  0.0, textPt, helloStr);
	p.String( 90.0, textPt, helloStr);
	p.SetFontStyle(JColorManager::GetYellowColor());
	p.String(180.0, textPt, helloStr);
	p.String(270.0, textPt, helloStr);

	p.SetPenColor(JColorManager::GetYellowColor());
	r.Set(0,11,80,91);
	p.Rect(r);
	r.Shrink(1,1);
	p.SetPenColor(JColorManager::GetBlueColor());
	p.Ellipse(r);
	r.Shrink(1,1);
	p.SetPenColor(JColorManager::GetRedColor());
	p.Arc(r, 270.0-45.0, -270.0);

	JPolygon poly;
	poly.AppendItem(JPoint(0,85));
	poly.AppendItem(JPoint(10,85));
	poly.AppendItem(JPoint(5,95));
	p.Polygon(poly);

	p.Line(0,100, 2,98);
	p.LineTo(4,100);
	p.LineTo(2,102);
	p.LineTo(0,100);

	poly.SetItem(1, JPoint(0,5));
	poly.SetItem(2, JPoint(2,0));
	poly.SetItem(3, JPoint(4,5));
	p.Polygon(2,105, poly);

	// test filling rule

	p.SetPenColor(JColorManager::GetRedColor());
	p.SetFilling(true);

	JPolygon fillRulePoly;
	fillRulePoly.AppendItem(JPoint(175,45));
	fillRulePoly.AppendItem(JPoint(165,65));
	fillRulePoly.AppendItem(JPoint(190,50));
	fillRulePoly.AppendItem(JPoint(160,50));
	fillRulePoly.AppendItem(JPoint(185,65));
	p.Polygon(fillRulePoly);

	p.SetFilling(false);

	// dashed lines

	p.DrawDashedLines(true);

	p.SetPenColor(JColorManager::GetBlackColor());

	JArray<JSize> dashList;			// pixel rulers
	dashList.AppendItem(1);
	dashList.AppendItem(1);
	p.SetDashList(dashList);
	p.Line(100,110, 200,110);
	p.Line(100,114, 200,114);
	p.Line(100,118, 200,118);

	dashList.SetItem(1, 2);		// simple pattern
	dashList.SetItem(2, 3);
	p.SetDashList(dashList);
	p.Line(100,112, 200,112);

	p.SetFontStyle(JFontStyle(false, false, 1, false));
	p.String(130,155, JGetString("FontTest::TestWidget"));

	p.SetDashList(dashList, 3);		// test offset
	p.Line(100,116, 200,116);

	dashList.SetItem(1, 1);		// example with odd # of values from X manuals
	dashList.SetItem(2, 2);
	dashList.AppendItem(3);
	p.SetDashList(dashList);
	p.Line(100,120, 200,120);

	dashList.SetItem(1, 5);		// dash-dot pattern
	dashList.SetItem(2, 2);
	dashList.SetItem(3, 1);
	dashList.AppendItem(2);
	p.SetDashList(dashList);
	p.Line(100,122, 200,122);

	p.Ellipse(210,110, 20,20);
	p.DrawDashedLines(false);
	p.Ellipse(213,113, 14,14);
	p.Rect(207,107, 26,26);
	p.SetPenColor(JColorManager::GetYellowColor());
	p.DrawDashedLines(true);
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

	if (itsImageBuffer != nullptr && (dw != 0 || dh != 0))
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
	itsImageBuffer = nullptr;

	// create image

	const JRect bounds = GetBounds();
	JXImage* imageBuffer =
		jnew JXImage(GetDisplay(), bounds.width(), bounds.height());
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
			JGetUserNotification()->ReportError(JGetString("SecretMenuError::TestWidget"));
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
	const bool scrolled = ScrollForDrag(pt);

	JPainter* p = nullptr;
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
	JPainter* p = nullptr;
	if (button == kJXLeftButton && GetDragPainter(&p))	// no painter for multiple click
	{
		p->Rect(JRect(itsStartPt, itsPrevPt));
		DeleteDragPainter();
	}
}

/******************************************************************************
 HitSamePart

 ******************************************************************************/

bool
TestWidget::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	return (its2Rect.Contains(pt1) && its2Rect.Contains(pt2)) ||
			(its3Rect.Contains(pt1) && its3Rect.Contains(pt2));
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

bool
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

	bool hasURIList = false;

	std::cout << std::endl;
	std::cout << "Data types available from DND source:" << std::endl;
	std::cout << std::endl;

	for (const auto type : typeList)
	{
		std::cout << XGetAtomName(*display, type) << std::endl;

		if (type == GetSelectionManager()->GetURLXAtom())
		{
			hasURIList = true;
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
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (*action == dndMgr->GetDNDActionCopyXAtom())
	{
		std::cout << std::endl;
		std::cout << "Accepting the drop" << std::endl;
		std::cout << std::endl;

		if (!PrintSelectionText(dndMgr->GetDNDSelectionName(), time,
								GetSelectionManager()->GetMimePlainTextXAtom()))
		{
			PrintSelectionText(dndMgr->GetDNDSelectionName(), time,
							   GetSelectionManager()->GetMimePlainTextUTF8XAtom());
		}
		return true;
	}
	else
	{
		std::cout << std::endl;
		std::cout << "Not accepting the drop because the action isn't copy" << std::endl;
		std::cout << "Action: " << XGetAtomName(*display, *action) << std::endl;
		std::cout << std::endl;

		if (!PrintSelectionText(dndMgr->GetDNDSelectionName(), time,
								GetSelectionManager()->GetMimePlainTextXAtom()))
		{
			PrintSelectionText(dndMgr->GetDNDSelectionName(), time,
							   GetSelectionManager()->GetMimePlainTextUTF8XAtom());
		}

		return false;
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

//	JGetUserNotification()->DisplayMessage("testing");

	Atom textType = None, urlType = None;
	for (const auto type : typeList)
	{
		std::cout << XGetAtomName(*display, type) << std::endl;
		if (type == selMgr->GetMimePlainTextXAtom() ||
			type == selMgr->GetMimePlainTextUTF8XAtom())
		{
			textType = type;
		}
		else if (type == selMgr->GetURLXAtom())
		{
			urlType = type;
		}
	}
	std::cout << std::endl;

	if (textType != None)
	{
		PrintSelectionText(GetDNDManager()->GetDNDSelectionName(), time, textType);
	}

	if (urlType != None)
	{
		PrintFileNames(GetDNDManager()->GetDNDSelectionName(), time, urlType);
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

	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(kJXClipboardName, time, &typeList))
	{
		std::cout << std::endl;
		std::cout << "Data types available from the clipboard:" << std::endl;
		std::cout << std::endl;

		for (const auto type : typeList)
		{
			std::cout << XGetAtomName(*display, type) << std::endl;
		}

		for (const auto type : typeList)
		{
			if (type == XA_STRING ||
				type == selMgr->GetUtf8StringXAtom() ||
				type == selMgr->GetMimePlainTextXAtom() ||
				type == selMgr->GetMimePlainTextUTF8XAtom())
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

bool
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
			returnType == selMgr->GetMimePlainTextXAtom() ||
			returnType == selMgr->GetMimePlainTextUTF8XAtom())
		{
			std::cout << "Data is available as " << XGetAtomName(*display, type) << ":" << std::endl << std::endl;
			std::cout << "====================" << std::endl;
			std::cout.write((char*) data, dataLength);
			std::cout << std::endl << "====================" << std::endl << std::endl;
		}
		else
		{
			std::cout << "Data has unrecognized return type:  ";
			std::cout << XGetAtomName(*(GetDisplay()), returnType) << std::endl;
			std::cout << "Trying to print it anyway:" << std::endl << std::endl;
			std::cout << "=========================" << std::endl;
			std::cout.write((char*) data, dataLength);
			std::cout << std::endl << "=========================" << std::endl << std::endl;
		}

		selMgr->DeleteData(&data, delMethod);
		return true;
	}
	else
	{
		std::cout << "Data could not be retrieved as " << XGetAtomName(*display, type) << "." << std::endl << std::endl;
		return false;
	}
}

/******************************************************************************
 PrintFileNames (private)

 ******************************************************************************/

void
TestWidget::PrintFileNames
	(
	const Atom selectionName,
	const Time time,
	const Atom type
	)
	const
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(selectionName, time, type,
						&returnType, &data, &dataLength, &delMethod))
	{
		if (returnType == type)
		{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);

			if (!fileNameList.IsEmpty())
			{
				std::cout << "File/directory names:" << std::endl << std::endl;
				for (const auto* f : fileNameList)
				{
					std::cout << *f << std::endl;
				}
				std::cout << std::endl << std::endl;
			}

			if (!urlList.IsEmpty())
			{
				std::cout << "Unconvertable URLs:" << std::endl << std::endl;
				for (const auto* u : urlList)
				{
					std::cout << *u << std::endl;
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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
/*
	std::cout << std::endl;
	std::cout << "Shift pressed  : " << modifiers.shift()   << std::endl;
	std::cout << "Control pressed: " << modifiers.control() << std::endl;
	std::cout << "Meta pressed   : " << modifiers.meta()    << std::endl;
*/
	JXScrollableWidget::HandleKeyPress(c, keySym, modifiers);
}

/******************************************************************************
 HandleClientMessage (virtual protected)

	This is for testing purposes.  It prints out every ClientMessage
	that it receives.

 ******************************************************************************/

bool
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
		for (char c : clientMessage.data.b)
		{
			std::cout << c << ' ';
		}
	}
	else if (clientMessage.format == 16)
	{
		for (short v : clientMessage.data.s)
		{
			std::cout << v << ' ';
		}
	}
	else
	{
		for (long v : clientMessage.data.l)
		{
			std::cout << v << ' ';
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

	itsActionsMenu->SetItemEnabled(kShowHideQuitCmd,
								  itsQuitButton != nullptr);
	itsActionsMenu->SetItemEnabled(kActDeactQuitCmd,
								  itsQuitButton != nullptr);
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
		itsActionsMenu->SetItemText(kShowHideCmd, JGetString("ShowMenuItem::TestWidget"));
	}
	else if (index == kShowHideCmd)
	{
		Show();
		itsActionsMenu->SetItemText(kShowHideCmd, JGetString("HideMenuItem::TestWidget"));
	}

	else if (index == kActDeactCmd && IsActive())
	{
		Deactivate();
		itsActionsMenu->SetItemText(kActDeactCmd, JGetString("ActivateMenuItem::TestWidget"));
	}
	else if (index == kActDeactCmd)
	{
		Activate();
		itsActionsMenu->SetItemText(kActDeactCmd, JGetString("DeactivateMenuItem::TestWidget"));
	}

	else if (index == kShowHideQuitCmd && itsQuitButton != nullptr &&
			 itsQuitButton->WouldBeVisible())
	{
		itsQuitButton->Hide();
		itsActionsMenu->SetItemText(kShowHideQuitCmd, JGetString("ShowQuitMenuItem::TestWidget"));
	}
	else if (index == kShowHideQuitCmd && itsQuitButton != nullptr)
	{
		itsQuitButton->Show();
		itsActionsMenu->SetItemText(kShowHideQuitCmd, JGetString("HideQuitMenuItem::TestWidget"));
	}

	else if (index == kActDeactQuitCmd && itsQuitButton != nullptr &&
			 itsQuitButton->WouldBeActive())
	{
		itsQuitButton->Deactivate();
		itsActionsMenu->SetItemText(kActDeactQuitCmd, JGetString("ActivateQuitMenuItem::TestWidget"));
	}
	else if (index == kActDeactQuitCmd && itsQuitButton != nullptr)
	{
		itsQuitButton->Activate();
		itsActionsMenu->SetItemText(kActDeactQuitCmd, JGetString("DeactivateQuitMenuItem::TestWidget"));
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
	auto* dlog = jnew ResizeWidgetDialog(this);

	if (dlog->DoDialog())
	{
		JCoordinate w,h;
		dlog->GetNewSize(&w, &h);
		SetBounds(w,h);
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
	JXWindowIcon* windowIcon = nullptr;

	if (GetWindow()->GetIconWidget(&windowIcon) &&
		sender == windowIcon && message->Is(JXWindowIcon::kAcceptDrop))
	{
		auto& data = dynamic_cast<JXWindowIcon::AcceptDrop&>(*message);
		if (!data.WillAcceptDrop())
		{
			data.ShouldAcceptDrop(
				WillAcceptDrop(data.GetTypeList(), data.GetActionPtr(),
							   data.GetPoint(), data.GetTime(), data.GetSource()));
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
	menu->SetUpdateAction(JXMenu::kDisableNone);

	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	GetXFontManager()->GetXFontNames(JRegex("^-.*-(courier|helvetica)-.*$"),
									   &fontList);
	for (const auto* fontName : fontList)
	{
		menu->AppendItem(*fontName);
	}

	menu->AppendItem(JGetString("XFontMenuLastItem::TestWidget"));
	const JSize count = menu->GetItemCount();
	menu->SetItemFontStyle(count, JFontStyle(true, false, 0, false,
											 JColorManager::GetBlackColor()));
	if (count > 1)
	{
		menu->ShowSeparatorAfter(count-1);
	}
}
