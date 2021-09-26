/******************************************************************************
 JXTabGroup.cpp

	Displays a list of tabs along any one of the four edges.  The tabs
	select items in a JXCardFile.  The tabs can be scrolled with the mouse
	wheel.

	Please use this class responsibly.  If there are more tabs than will
	fit along the edge, consider using JXStringList or JXTreeListWidget
	instead.

	BASE CLASS = JXWidget

	Copyright (C) 2002-08 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXTabGroup.h"
#include "jx-af/jx/JXCardFile.h"
#include "jx-af/jx/JXScrollTabsTask.h"
#include "jx-af/jx/JXTextMenu.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXWindowPainter.h"
#include "jx-af/jx/jXPainterUtil.h"
#include "jx-af/jx/JXFontManager.h"
#include "jx-af/jx/JXColorManager.h"
#include "jx-af/jx/JXImage.h"
#include "jx-af/jx/JXImageCache.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

#include <jx_tab_close.xpm>
#include <jx_tab_close_pushed.xpm>

const JCoordinate kBorderWidth      = kJXDefaultBorderWidth;
const JCoordinate kTextMargin       = 4;
const JCoordinate kSelMargin        = 2;
const JCoordinate kArrowWidth       = 15;
const JCoordinate kCloseMarginWidth = 5;

const JFloat kInitialScrollDelay    = 0.2;
const JFloat kContinuousScrollDelay = 0.05;

const JSize kSelGrayPercentage = 70;

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JUtf8Byte kSetupDataEndDelimiter  = '\1';

// Context menu

#define kShowTopAction		"ShowTop::JXTabGroup"
#define kShowBottomAction	"ShowBottom::JXTabGroup"
#define kShowLeftAction		"ShowLeft::JXTabGroup"
#define kShowRightAction	"ShowRight::JXTabGroup"

static const JUtf8Byte* kContextMenuStr =
	"    Show on top    %i" kShowTopAction
	"  | Show on bottom %i" kShowBottomAction
	"%l| Show at left   %i" kShowLeftAction
	"  | Show at right  %i" kShowRightAction;

enum
{
	kContextTopCmd = 1,
	kContextBottomCmd,
	kContextLeftCmd,
	kContextRightCmd
};

// JBroadcaster message types

const JUtf8Byte* JXTabGroup::kAppearanceChanged = "AppearanceChanged::JXTabGroup";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTabGroup::JXTabGroup
	(
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
	itsEdge(kTop),
	itsFont(JFontManager::GetDefaultFont()),
	itsCanScrollUpFlag(false),
	itsCanScrollDownFlag(false),
	itsFirstDrawIndex(1),
	itsLastDrawIndex(1),
	itsContextMenu(nullptr),
	itsPrevTabIndex(0),
	itsDragAction(kInvalidClick),
	itsScrollUpPushedFlag(false),
	itsScrollDownPushedFlag(false),
	itsMouseIndex(0),
	itsClosePushedFlag(false)
{
	itsTitles = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsTitles != nullptr );

	itsTabInfoList = jnew JArray<TabInfo>;
	assert( itsTabInfoList != nullptr );

	JXImageCache* cache = GetDisplay()->GetImageCache();
	itsCloseImage       = cache->GetImage(jx_tab_close);
	itsClosePushedImage = cache->GetImage(jx_tab_close_pushed);

	itsTabRects = jnew JArray<JRect>;
	assert( itsTabRects != nullptr );

	itsCardFile = jnew JXCardFile(this, kHElastic, kVElastic, 0,0, 100,100);
	assert( itsCardFile != nullptr );
	PlaceCardFile();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTabGroup::~JXTabGroup()
{
	jdelete itsTitles;
	jdelete itsTabInfoList;
	jdelete itsTabRects;
}

/******************************************************************************
 Tab title margins

 ******************************************************************************/

JCoordinate
JXTabGroup::GetTabTitlePreMargin
	(
	const JIndex index
	)
	const
{
	return (itsTabInfoList->GetElement(index)).preMargin - kTextMargin;
}

void
JXTabGroup::SetTabTitlePreMargin
	(
	const JIndex		index,
	const JCoordinate	margin
	)
{
	TabInfo info   = itsTabInfoList->GetElement(index);
	info.preMargin = kTextMargin + margin;
	itsTabInfoList->SetElement(index, info);
	Refresh();
}

JCoordinate
JXTabGroup::GetTabTitlePostMargin
	(
	const JIndex index
	)
	const
{
	return (itsTabInfoList->GetElement(index)).postMargin - kTextMargin;
}

void
JXTabGroup::SetTabTitlePostMargin
	(
	const JIndex		index,
	const JCoordinate	margin
	)
{
	TabInfo info    = itsTabInfoList->GetElement(index);
	info.postMargin = kTextMargin + margin;
	itsTabInfoList->SetElement(index, info);
	Refresh();
}

JXTabGroup::TabInfo::TabInfo()
	:
	closable(false), preMargin(kTextMargin), postMargin(kTextMargin)
{
};

JXTabGroup::TabInfo::TabInfo
	(
	const bool closable
	)
	:
	closable(closable), preMargin(kTextMargin), postMargin(kTextMargin)
{
};

/******************************************************************************
 ShowTab

	If the current card is willing to disappear, then we show the requested
	one and return true.  Otherwise, we return false.

 ******************************************************************************/

bool
JXTabGroup::ShowTab
	(
	JXContainer* card
	)
{
	JIndex index;
	if (itsCardFile->GetCardIndex(card, &index))
	{
		return ShowTab(index);
	}
	else
	{
		return false;
	}
}

bool
JXTabGroup::ShowTab
	(
	const JIndex index
	)
{
	JIndex selIndex;
	const bool hasSelection = itsCardFile->GetCurrentCardIndex(&selIndex);
	if (hasSelection && selIndex == index)
	{
		return true;
	}
	else if (hasSelection)
	{
		itsPrevTabIndex = selIndex;
	}

	if (itsCardFile->ShowCard(index))
	{
		JIndex i;
		const bool hasIndex = GetCurrentTabIndex(&i);
		if (hasIndex && i < itsFirstDrawIndex)
		{
			itsFirstDrawIndex = i;
		}
		else if (hasIndex && i > itsFirstDrawIndex)
		{
			ScrollUpToTab(i);
		}

		Refresh();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ShowPreviousTab

 ******************************************************************************/

void
JXTabGroup::ShowPreviousTab()
{
	JIndex index;
	if (!GetCurrentTabIndex(&index))
	{
		index = 2;
	}

	index--;
	if (index == 0)
	{
		index = GetTabCount();
	}

	ShowTab(index);
}

/******************************************************************************
 ShowNextTab

 ******************************************************************************/

void
JXTabGroup::ShowNextTab()
{
	JIndex index;
	if (!GetCurrentTabIndex(&index))
	{
		index = 0;
	}

	index++;
	if (index > GetTabCount())
	{
		index = 1;
	}

	ShowTab(index);
}

/******************************************************************************
 ScrollTabsIntoView

 ******************************************************************************/

void
JXTabGroup::ScrollTabsIntoView()
{
	Redraw();
	if (itsCanScrollUpFlag && !itsCanScrollDownFlag)
	{
		while (!itsCanScrollDownFlag && itsFirstDrawIndex > 1)
		{
			itsFirstDrawIndex--;
			Redraw();
		}

		if (itsCanScrollDownFlag)
		{
			itsFirstDrawIndex++;
			Refresh();
		}
	}
}

/******************************************************************************
 InsertTab

	The first version creates a new card, inserts it at the specified index,
	and returns it.

	The second version lets you provide the card, so that you can use
	a derived class to add intelligence.  The card's enclosure must be
	GetCardEnclosure().  We own the card after you give it to us.

 ******************************************************************************/

JXContainer*
JXTabGroup::InsertTab
	(
	const JIndex	index,
	const JString&	title,
	const bool	closeable
	)
{
	Refresh();
	itsTitles->InsertAtIndex(index, title);
	itsTabInfoList->InsertElementAtIndex(index, TabInfo(closeable));
	return itsCardFile->InsertCard(index);
}

void
JXTabGroup::InsertTab
	(
	const JIndex	index,
	const JString&	title,
	JXWidgetSet*	card,
	const bool	closeable
	)
{
	Refresh();
	itsTitles->InsertAtIndex(index, title);
	itsTabInfoList->InsertElementAtIndex(index, TabInfo(closeable));
	itsCardFile->InsertCard(index, card);
}

/******************************************************************************
 RemoveTab

	Removes the specified tab and returns a pointer to the card so you can
	keep track of it.

 ******************************************************************************/

JXContainer*
JXTabGroup::RemoveTab
	(
	const JIndex index
	)
{
	JIndex selIndex;
	const bool hasSelection = itsCardFile->GetCurrentCardIndex(&selIndex);
	if (hasSelection && index == selIndex &&
		itsPrevTabIndex > 0 && itsPrevTabIndex != index)
	{
		ShowTab(itsPrevTabIndex);
	}
	else if (hasSelection && index == selIndex && index > 1)
	{
		ShowTab(index-1);
	}
	else if (hasSelection && index == selIndex && index < GetTabCount())
	{
		ShowTab(index+1);
	}

	itsPrevTabIndex = 0;

	if (itsCanScrollUpFlag && !itsCanScrollDownFlag)
	{
		auto* task = jnew JXScrollTabsTask(this);
		assert( task != nullptr );
		task->Go();
	}

	Refresh();
	itsTitles->DeleteElement(index);
	itsTabInfoList->RemoveElement(index);
	return itsCardFile->RemoveCard(index);
}

/******************************************************************************
 PlaceCardFile (private)

 ******************************************************************************/

void
JXTabGroup::PlaceCardFile()
{
	const JSize h = kSelMargin + kBorderWidth + 2*kTextMargin +
					itsFont.GetLineHeight(GetFontManager());

	JRect r = GetAperture();
	if (itsEdge == kTop)
	{
		r.top += h;
	}
	else if (itsEdge == kLeft)
	{
		r.left += h;
	}
	else if (itsEdge == kBottom)
	{
		r.bottom -= h;
	}
	else if (itsEdge == kRight)
	{
		r.right -= h;
	}
	else
	{
		assert( 0 );
	}

	r.Shrink(kBorderWidth, kBorderWidth);
	itsCardFile->Place(r.left, r.top);
	itsCardFile->SetSize(r.width(), r.height());
}

/******************************************************************************
 ScrollUpToTab (private)

 ******************************************************************************/

void
JXTabGroup::ScrollUpToTab
	(
	const JIndex index
	)
{
	assert( itsTitles->IndexValid(index) );
	assert( index > itsFirstDrawIndex );

	const JCoordinate scrollArrowWidth = 2*(kArrowWidth + kBorderWidth);

	const JRect ap        = GetAperture();
	const JCoordinate min = (itsEdge == kTop || itsEdge == kBottom ? ap.left : ap.top);
	const JCoordinate max = (itsEdge == kTop || itsEdge == kBottom ? ap.right : ap.bottom);
	JCoordinate left      = min + kSelMargin;
	JCoordinate right     = left;
	JArray<JCoordinate> widthList;

	const JSize count  = itsTitles->GetElementCount();
	bool offScreen = false;
	for (JIndex i=itsFirstDrawIndex; i<=index; i++)
	{
		const TabInfo info = itsTabInfoList->GetElement(index);

		right += 2*kBorderWidth + info.preMargin + info.postMargin +
				 itsFont.GetStringWidth(GetFontManager(), *(itsTitles->GetElement(i)));
		if (info.closable)
		{
			right += kCloseMarginWidth + itsCloseImage->GetWidth();
		}
		widthList.AppendElement(right - left);

		if (!offScreen &&
			right >= max - scrollArrowWidth &&
			!(itsFirstDrawIndex == 1 && i == count && right <= max))
		{
			offScreen = true;
		}

		left = right;
	}

	if (offScreen)
	{
		JIndex i = 1;
		while (right > max - scrollArrowWidth && itsFirstDrawIndex < index)
		{
			right -= widthList.GetElement(i);
			itsFirstDrawIndex++;
			i++;
		}
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTabGroup::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect ap = GetAperture();

	p.SetFont(itsFont);
	const JSize lineHeight = p.GetLineHeight();
	const JSize tabHeight  = 2*(kBorderWidth + kTextMargin) + lineHeight;

	JIndex selIndex;
	JRect selRect;
	const bool hasSelection = itsCardFile->GetCurrentCardIndex(&selIndex);

	itsTabRects->RemoveAll();
	itsCanScrollUpFlag   = itsFirstDrawIndex > 1;
	itsCanScrollDownFlag = false;

	const JCoordinate scrollArrowWidth = 2*(kArrowWidth + kBorderWidth);

	const JSize count = itsTitles->GetElementCount();
	itsLastDrawIndex  = JMax(count, itsFirstDrawIndex);

	if (itsEdge == kTop)
	{
		JRect r(ap.top + kSelMargin,             ap.left + kSelMargin,
				ap.top + kSelMargin + tabHeight, ap.left + kSelMargin);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
		{
			const JString* title = itsTitles->GetElement(i);
			const bool isSel = hasSelection && i == selIndex;
			const TabInfo info   = itsTabInfoList->GetElement(i);

			r.right += 2*kBorderWidth + info.preMargin +info.postMargin + p.GetStringWidth(*title);
			if (info.closable)
			{
				r.right += kCloseMarginWidth + itsCloseImage->GetWidth();
			}
			JPoint titlePt(r.left + kBorderWidth + info.preMargin,
						   r.top  + kBorderWidth + kTextMargin);
			if (isSel)
			{
//				titlePt.y -= kSelMargin;
				r.top     -= kSelMargin;
				r.Expand(kSelMargin, 0);

				selRect = r;
			}

			if (isSel)
			{
				p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
				p.SetFilling(true);
				p.JPainter::Rect(r);
				p.SetFilling(false);
			}
			else
			{
				DrawTabBorder(p, r, false);
			}
			p.JPainter::String(titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
			{
				r.top += kSelMargin;
				r.Shrink(kSelMargin, 0);
			}

			r.Shrink(kBorderWidth, kBorderWidth);
			DrawTab(i, p, r, itsEdge);
			DrawCloseButton(i, p, r);
			r.Expand(kBorderWidth, kBorderWidth);

			if (r.right >= ap.right - scrollArrowWidth)
			{
				if (itsFirstDrawIndex == 1 && i == count && r.right <= ap.right)
				{
					break;
				}
				itsCanScrollDownFlag = itsFirstDrawIndex < count;
				itsLastDrawIndex     = i;
				if (r.right > ap.right - scrollArrowWidth && i > itsFirstDrawIndex)
				{
					itsLastDrawIndex--;
				}
				break;
			}

			r.left = r.right;
		}
	}

	else if (itsEdge == kLeft)
	{
		JRect r(ap.bottom - kSelMargin, ap.left + kSelMargin,
				ap.bottom - kSelMargin, ap.left + kSelMargin + tabHeight);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
		{
			const JString* title = itsTitles->GetElement(i);
			const bool isSel = hasSelection && i == selIndex;
			const TabInfo info   = itsTabInfoList->GetElement(i);

			r.top -= 2*kBorderWidth + info.preMargin + info.postMargin + p.GetStringWidth(*title);
			if (info.closable)
			{
				r.top -= kCloseMarginWidth + itsCloseImage->GetWidth();
			}
			JPoint titlePt(r.left   + kBorderWidth + kTextMargin,
						   r.bottom - kBorderWidth - info.preMargin);
			if (isSel)
			{
//				titlePt.x -= kSelMargin;
				r.left    -= kSelMargin;
				r.Expand(0, kSelMargin);

				selRect = r;
			}

			if (isSel)
			{
				p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
				p.SetFilling(true);
				p.JPainter::Rect(r);
				p.SetFilling(false);
			}
			else
			{
				DrawTabBorder(p, r, false);
			}
			p.JPainter::String(90, titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
			{
				r.left += kSelMargin;
				r.Shrink(0, kSelMargin);
			}

			r.Shrink(kBorderWidth, kBorderWidth);
			DrawTab(i, p, r, itsEdge);
			DrawCloseButton(i, p, r);
			r.Expand(kBorderWidth, kBorderWidth);

			if (r.top <= ap.top + scrollArrowWidth)
			{
				if (itsFirstDrawIndex == 1 && i == count && r.top >= ap.top)
				{
					break;
				}
				itsCanScrollDownFlag = itsFirstDrawIndex < count;
				itsLastDrawIndex     = i;
				if (r.top < ap.top + scrollArrowWidth && i > itsFirstDrawIndex)
				{
					itsLastDrawIndex--;
				}
				break;
			}

			r.bottom = r.top;
		}
	}

	else if (itsEdge == kBottom)
	{
		JRect r(ap.bottom - kSelMargin - tabHeight, ap.left + kSelMargin,
				ap.bottom - kSelMargin,             ap.left + kSelMargin);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
		{
			const JString* title = itsTitles->GetElement(i);
			const bool isSel = hasSelection && i == selIndex;
			const TabInfo info   = itsTabInfoList->GetElement(i);

			r.right += 2*kBorderWidth + info.preMargin + info.postMargin + p.GetStringWidth(*title);
			if (info.closable)
			{
				r.right += kCloseMarginWidth + itsCloseImage->GetWidth();
			}
			JPoint titlePt(r.left + kBorderWidth + info.preMargin,
						   r.top  + kBorderWidth + kTextMargin);
			if (isSel)
			{
//				titlePt.y += kSelMargin;
				r.bottom  += kSelMargin;
				r.Expand(kSelMargin, 0);

				selRect = r;
			}

			if (isSel)
			{
				p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
				p.SetFilling(true);
				p.JPainter::Rect(r);
				p.SetFilling(false);
			}
			else
			{
				DrawTabBorder(p, r, false);
			}
			p.JPainter::String(titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
			{
				r.bottom -= kSelMargin;
				r.Shrink(kSelMargin, 0);
			}

			r.Shrink(kBorderWidth, kBorderWidth);
			DrawTab(i, p, r, itsEdge);
			DrawCloseButton(i, p, r);
			r.Expand(kBorderWidth, kBorderWidth);

			if (r.right >= ap.right - scrollArrowWidth)
			{
				if (itsFirstDrawIndex == 1 && i == count && r.right <= ap.right)
				{
					break;
				}
				itsCanScrollDownFlag = itsFirstDrawIndex < count;
				itsLastDrawIndex     = i;
				if (r.right > ap.right - scrollArrowWidth && i > itsFirstDrawIndex)
				{
					itsLastDrawIndex--;
				}
				break;
			}

			r.left = r.right;
		}
	}

	else if (itsEdge == kRight)
	{
		JRect r(ap.top + kSelMargin, ap.right - kSelMargin - tabHeight,
				ap.top + kSelMargin, ap.right - kSelMargin);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
		{
			const JString* title = itsTitles->GetElement(i);
			const bool isSel = hasSelection && i == selIndex;
			const TabInfo info   = itsTabInfoList->GetElement(i);

			r.bottom += 2*kBorderWidth + info.preMargin + info.postMargin + p.GetStringWidth(*title);
			if (info.closable)
			{
				r.bottom += kCloseMarginWidth + itsCloseImage->GetWidth();
			}
			JPoint titlePt(r.right - kBorderWidth - kTextMargin,
						   r.top   + kBorderWidth + info.preMargin);
			if (isSel)
			{
//				titlePt.x += kSelMargin;
				r.right   += kSelMargin;
				r.Expand(0, kSelMargin);

				selRect = r;
			}

			if (isSel)
			{
				p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
				p.SetFilling(true);
				p.JPainter::Rect(r);
				p.SetFilling(false);
			}
			else
			{
				DrawTabBorder(p, r, false);
			}
			p.JPainter::String(-90, titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
			{
				r.right -= kSelMargin;
				r.Shrink(0, kSelMargin);
			}

			r.Shrink(kBorderWidth, kBorderWidth);
			DrawTab(i, p, r, itsEdge);
			DrawCloseButton(i, p, r);
			r.Expand(kBorderWidth, kBorderWidth);

			if (r.bottom >= ap.bottom - scrollArrowWidth)
			{
				if (itsFirstDrawIndex == 1 && i == count && r.bottom <= ap.bottom)
				{
					break;
				}
				itsCanScrollDownFlag = itsFirstDrawIndex < count;
				itsLastDrawIndex     = i;
				if (r.bottom > ap.bottom - scrollArrowWidth && i > itsFirstDrawIndex)
				{
					itsLastDrawIndex--;
				}
				break;
			}

			r.top = r.bottom;
		}
	}

	JRect r = itsCardFile->GetFrame();
	r.Expand(kBorderWidth, kBorderWidth);
	JXDrawUpFrame(p, r, kBorderWidth);

	if (!selRect.IsEmpty())
	{
		DrawTabBorder(p, selRect, true);
	}

	DrawScrollButtons(p, lineHeight);
}

/******************************************************************************
 DrawCloseButton (private)

 ******************************************************************************/

void
JXTabGroup::DrawCloseButton
	(
	const JIndex		index,
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (index != itsMouseIndex)
	{
		return;
	}
	else if (!TabCanClose(index))
	{
		itsCloseRect.Set(0,0,0,0);
		return;
	}

	if (itsEdge == kTop || itsEdge == kBottom)
	{
		itsCloseRect.top    = rect.ycenter() - itsCloseImage->GetHeight()/2;
		itsCloseRect.bottom = itsCloseRect.top + itsCloseImage->GetHeight();
		itsCloseRect.right  = rect.right - kCloseMarginWidth;
		itsCloseRect.left   = itsCloseRect.right - itsCloseImage->GetWidth();
	}
	else if (itsEdge == kLeft)
	{
		itsCloseRect.top    = rect.top + kCloseMarginWidth;
		itsCloseRect.bottom = itsCloseRect.top + itsCloseImage->GetHeight();
		itsCloseRect.left   = rect.xcenter() - itsCloseImage->GetWidth()/2;
		itsCloseRect.right  = itsCloseRect.left + itsCloseImage->GetWidth();
	}
	else	// itsEdge == kRight
	{
		itsCloseRect.bottom = rect.bottom - kCloseMarginWidth;
		itsCloseRect.top    = itsCloseRect.bottom - itsCloseImage->GetHeight();
		itsCloseRect.left   = rect.xcenter() - itsCloseImage->GetWidth()/2;
		itsCloseRect.right  = itsCloseRect.left + itsCloseImage->GetWidth();
	}

	if (itsClosePushedFlag)
	{
		p.Image(*itsClosePushedImage, itsClosePushedImage->GetBounds(), itsCloseRect);
	}
	else
	{
		p.Image(*itsCloseImage, itsCloseImage->GetBounds(), itsCloseRect);
	}
}

/******************************************************************************
 DrawTab (virtual protected)

 ******************************************************************************/

void
JXTabGroup::DrawTab
	(
	const JIndex		index,
	JXWindowPainter&	p,
	const JRect&		rect,
	const Edge			edge
	)
{
}

/******************************************************************************
 DrawTabBorder (private)

 ******************************************************************************/

void
JXTabGroup::DrawTabBorder
	(
	JXWindowPainter&	p,
	const JRect&		rect,
	const bool		isSelected
	)
{
	JXDrawUpFrame(p, rect, kBorderWidth);

	if (itsEdge == kTop)
	{
		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.JPainter::Point(rect.topLeft());
		p.JPainter::Point(rect.topRight() + JPoint(-1,0));
		p.JPainter::Point(rect.topRight() + JPoint(-2,0));
		p.JPainter::Point(rect.topRight() + JPoint(-1,1));
		p.JPainter::Point(rect.topRight() + JPoint(-1,2));
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.JPainter::Point(rect.topLeft() + JPoint(kBorderWidth, kBorderWidth));
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.JPainter::Point(rect.topRight() + JPoint(-kBorderWidth-1, kBorderWidth));

		if (isSelected)
		{
			JRect r(rect.bottom - kBorderWidth, rect.left  + kBorderWidth,
					rect.bottom,                rect.right - kBorderWidth);
			p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
			p.JPainter::Rect(r);
			p.JPainter::Point(rect.topLeft() + JPoint(-1,kSelMargin+kBorderWidth));
			p.SetPenColor(JColorManager::Get3DLightColor());
			p.JPainter::Point(rect.bottomLeft()  + JPoint(1,-1));
			p.JPainter::Point(rect.bottomRight() + JPoint(-2,-1));
			p.JPainter::Point(rect.bottomRight() + JPoint(-1,-2));
			p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
		}
	}

	else if (itsEdge == kLeft)
	{
		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.JPainter::Point(rect.topLeft());
		p.JPainter::Point(rect.bottomLeft() + JPoint(0,-1));
		p.JPainter::Point(rect.bottomLeft() + JPoint(0,-2));
		p.JPainter::Point(rect.bottomLeft() + JPoint(1,-1));
		p.JPainter::Point(rect.bottomLeft() + JPoint(2,-1));
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.JPainter::Point(rect.topLeft() + JPoint(kBorderWidth, kBorderWidth));
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.JPainter::Point(rect.bottomLeft() + JPoint(kBorderWidth, -kBorderWidth-1));

		if (isSelected)
		{
			JRect r(rect.top    + kBorderWidth, rect.right - kBorderWidth,
					rect.bottom - kBorderWidth, rect.right);
			p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
			p.JPainter::Rect(r);
			p.JPainter::Point(rect.topLeft() + JPoint(kSelMargin+kBorderWidth,-1));
			p.SetPenColor(JColorManager::Get3DLightColor());
			p.JPainter::Point(rect.topRight() + JPoint(-1,1));
			if (rect.bottom < (GetAperture()).bottom)
			{
				p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
				p.JPainter::Point(rect.bottomRight() + JPoint(-2,-1));
				p.JPainter::Point(rect.bottomRight() + JPoint(-1,-2));
			}
		}
	}

	else if (itsEdge == kBottom)
	{
		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.JPainter::Point(rect.bottomLeft()  + JPoint(0,-1));
		p.JPainter::Point(rect.bottomLeft()  + JPoint(0,-2));
		p.JPainter::Point(rect.bottomLeft()  + JPoint(1,-1));
		p.JPainter::Point(rect.bottomLeft()  + JPoint(2,-1));
		p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.JPainter::Point(rect.bottomLeft() + JPoint(kBorderWidth, -kBorderWidth-1));
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.JPainter::Point(rect.bottomRight() + JPoint(-kBorderWidth-1, -kBorderWidth-1));

		if (isSelected)
		{
			JRect r(rect.top,                rect.left  + kBorderWidth,
					rect.top + kBorderWidth, rect.right - kBorderWidth);
			p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
			p.JPainter::Rect(r);
			p.SetPenColor(JColorManager::Get3DShadeColor());
			if (rect.left > (GetAperture()).left)
			{
				p.JPainter::Point(rect.topLeft());
			}
			p.JPainter::Point(rect.topRight() + JPoint(-1,0));
			p.JPainter::Point(rect.topRight() + JPoint(-2,0));
			p.JPainter::Point(rect.topRight() + JPoint(-2,1));
		}
	}

	else if (itsEdge == kRight)
	{
		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.JPainter::Point(rect.topRight() + JPoint(-1,0));
		p.JPainter::Point(rect.topRight() + JPoint(-2,0));
		p.JPainter::Point(rect.topRight() + JPoint(-1,1));
		p.JPainter::Point(rect.topRight() + JPoint(-1,2));
		p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.JPainter::Point(rect.topRight() + JPoint(-kBorderWidth-1, kBorderWidth));
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.JPainter::Point(rect.bottomRight() + JPoint(-kBorderWidth-1, -kBorderWidth-1));

		if (isSelected)
		{
			JRect r(rect.top    + kBorderWidth, rect.left,
					rect.bottom - kBorderWidth, rect.left + kBorderWidth);
			p.SetPenColor(JColorManager::GetGrayColor(kSelGrayPercentage));
			p.JPainter::Rect(r);
			p.SetPenColor(JColorManager::Get3DShadeColor());
			if (rect.top > (GetAperture()).top)
			{
				p.JPainter::Point(rect.topLeft());
			}
			p.JPainter::Point(rect.bottomLeft() + JPoint(0,-1));
			p.JPainter::Point(rect.bottomLeft() + JPoint(0,-2));
			p.JPainter::Point(rect.bottomLeft() + JPoint(1,-2));
		}
	}
}

/******************************************************************************
 DrawScrollButtons (private)

 ******************************************************************************/

void
JXTabGroup::DrawScrollButtons
	(
	JXWindowPainter&	p,
	const JCoordinate	lineHeight
	)
{
	itsScrollUpRect = itsScrollDownRect = JRect(0,0,0,0);
	if (!itsCanScrollUpFlag && !itsCanScrollDownFlag)
	{
		return;
	}

	const JCoordinate w = 2*(kArrowWidth + kBorderWidth);
	const JCoordinate h = kSelMargin + kBorderWidth + 2*kTextMargin + lineHeight;

	const JRect ap = GetAperture();
	if (itsEdge == kTop)
	{
		JRect r(ap.top,     ap.right - w,
				ap.top + h, ap.right);

		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.SetFilling(true);
		p.JPainter::Rect(r);
		p.SetFilling(false);

		r.Shrink(kBorderWidth, 0);
		r.top = r.bottom = r.ycenter();
		r.top    -= kArrowWidth/2;
		r.bottom += kArrowWidth/2 + kArrowWidth%2;

		r.right = r.left + kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
		{
			JXDrawDownArrowLeft(p, r, kBorderWidth);
		}
		else if (itsCanScrollUpFlag)
		{
			JXDrawUpArrowLeft(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowLeft(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollUpRect = r;

		r.left  += kArrowWidth;
		r.right += kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
		{
			JXDrawDownArrowRight(p, r, kBorderWidth);
		}
		else if (itsCanScrollDownFlag)
		{
			JXDrawUpArrowRight(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowRight(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollDownRect = r;
	}

	else if (itsEdge == kLeft)
	{
		JRect r(ap.top,     ap.left,
				ap.top + w, ap.left + h);

		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.SetFilling(true);
		p.JPainter::Rect(r);
		p.SetFilling(false);

		r.Shrink(0, kBorderWidth);
		r.left = r.right = r.xcenter();
		r.left  -= kArrowWidth/2;
		r.right += kArrowWidth/2 + kArrowWidth%2;

		r.bottom = r.top + kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
		{
			JXDrawDownArrowUp(p, r, kBorderWidth);
		}
		else if (itsCanScrollDownFlag)
		{
			JXDrawUpArrowUp(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowUp(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollDownRect = r;

		r.top    += kArrowWidth;
		r.bottom += kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
		{
			JXDrawDownArrowDown(p, r, kBorderWidth);
		}
		else if (itsCanScrollUpFlag)
		{
			JXDrawUpArrowDown(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowDown(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollUpRect = r;
	}

	else if (itsEdge == kBottom)
	{
		JRect r(ap.bottom - h, ap.right - w,
				ap.bottom,     ap.right);

		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.SetFilling(true);
		p.JPainter::Rect(r);
		p.SetFilling(false);

		r.Shrink(kBorderWidth, 0);
		r.top = r.bottom = r.ycenter();
		r.top    -= kArrowWidth/2;
		r.bottom += kArrowWidth/2 + kArrowWidth%2;

		r.right = r.left + kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
		{
			JXDrawDownArrowLeft(p, r, kBorderWidth);
		}
		else if (itsCanScrollUpFlag)
		{
			JXDrawUpArrowLeft(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowLeft(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollUpRect = r;

		r.left  += kArrowWidth;
		r.right += kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
		{
			JXDrawDownArrowRight(p, r, kBorderWidth);
		}
		else if (itsCanScrollDownFlag)
		{
			JXDrawUpArrowRight(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowRight(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollDownRect = r;
	}

	else if (itsEdge == kRight)
	{
		JRect r(ap.bottom - w, ap.right - h,
				ap.bottom,     ap.right);

		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.SetFilling(true);
		p.JPainter::Rect(r);
		p.SetFilling(false);

		r.Shrink(0, kBorderWidth);
		r.left = r.right = r.xcenter();
		r.left  -= kArrowWidth/2;
		r.right += kArrowWidth/2 + kArrowWidth%2;

		r.bottom = r.top + kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
		{
			JXDrawDownArrowUp(p, r, kBorderWidth);
		}
		else if (itsCanScrollUpFlag)
		{
			JXDrawUpArrowUp(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowUp(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollUpRect = r;

		r.top    += kArrowWidth;
		r.bottom += kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
		{
			JXDrawDownArrowDown(p, r, kBorderWidth);
		}
		else if (itsCanScrollDownFlag)
		{
			JXDrawUpArrowDown(p, r, kBorderWidth);
		}
		else
		{
			JXFillArrowDown(p, r, JColorManager::GetInactiveLabelColor());
		}
		itsScrollDownRect = r;
	}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXTabGroup::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXTabGroup::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidget::BoundsResized(dw,dh);

	if (((itsEdge == kTop || itsEdge == kBottom) && dw > 0) ||
		((itsEdge == kLeft || itsEdge == kRight) && dh > 0))
	{
		auto* task = jnew JXScrollTabsTask(this);
		assert( task != nullptr );
		task->Go();
	}
}

/******************************************************************************
 HandleMouseHere (virtual protected)

 ******************************************************************************/

void
JXTabGroup::HandleMouseHere
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	JIndex i;
	JRect r;
	bool found = FindTab(pt, &i, &r);

	if (found && i != itsMouseIndex)
	{
		itsMouseIndex = i;
		Refresh();
	}
	else if (!found && itsMouseIndex > 0)
	{
		HandleMouseLeave();
	}
}

/******************************************************************************
 HandleMouseLeave (virtual protected)

 ******************************************************************************/

void
JXTabGroup::HandleMouseLeave()
{
	if (itsMouseIndex > 0)
	{
		itsMouseIndex = 0;
		itsCloseRect.Set(0,0,0,0);
		Refresh();
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXTabGroup::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDragAction = kInvalidClick;

	if (button == kJXLeftButton && itsScrollUpRect.Contains(pt))
	{
		if (itsCanScrollUpFlag && itsFirstDrawIndex > 1)	// avoid left click when arrow disabled
		{
			itsDragAction         = kScrollUp;
			itsScrollUpPushedFlag = true;
			itsFirstDrawIndex--;
			Refresh();
			ScrollWait(kInitialScrollDelay);
			itsScrollUpPushedFlag = false;		// ignore first HandleMouseDrag()
		}
	}
	else if (button == kJXLeftButton && itsScrollDownRect.Contains(pt))
	{
		if (itsCanScrollDownFlag && itsFirstDrawIndex < GetTabCount())	// avoid left click when arrow disabled
		{
			itsDragAction           = kScrollDown;
			itsScrollDownPushedFlag = true;
			itsFirstDrawIndex++;
			Refresh();
			ScrollWait(kInitialScrollDelay);
			itsScrollDownPushedFlag = false;		// ignore first HandleMouseDrag()
		}
	}
	else if (button == kJXLeftButton &&
			 itsMouseIndex > 0 && itsCloseRect.Contains(pt))
	{
		itsDragAction      = kClose;
		itsClosePushedFlag = true;
		Refresh();
	}
	else if (button == kJXLeftButton)
	{
		JIndex i;
		JRect r;
		if (FindTab(pt, &i, &r))
		{
			ShowTab(i);
		}
	}
	else if (button == kJXRightButton)
	{
		CreateContextMenu();
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else
	{
		ScrollForWheel(button, modifiers);
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXTabGroup::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragAction == kScrollUp)
	{
		const bool newScrollUpPushedFlag = itsScrollUpRect.Contains(pt);
		if (itsScrollUpPushedFlag != newScrollUpPushedFlag)
		{
			itsScrollUpPushedFlag = newScrollUpPushedFlag;
			Redraw();
		}
		else if (itsScrollUpPushedFlag &&
				 itsCanScrollUpFlag && itsFirstDrawIndex > 1)
		{
			itsFirstDrawIndex--;
			Refresh();
			ScrollWait(kContinuousScrollDelay);
		}
	}
	else if (itsDragAction == kScrollDown)
	{
		const bool newScrollDownPushedFlag = itsScrollDownRect.Contains(pt);
		if (itsScrollDownPushedFlag != newScrollDownPushedFlag)
		{
			itsScrollDownPushedFlag = newScrollDownPushedFlag;
			Redraw();
		}
		else if (itsScrollDownPushedFlag &&
				 itsCanScrollDownFlag && itsFirstDrawIndex < GetTabCount())
		{
			itsFirstDrawIndex++;
			Refresh();
			ScrollWait(kContinuousScrollDelay);
		}
	}
	else if (itsDragAction == kClose)
	{
		const bool old = itsClosePushedFlag;
		itsClosePushedFlag = itsMouseIndex > 0 && itsCloseRect.Contains(pt);
		if ((!old &&  itsClosePushedFlag) ||
			( old && !itsClosePushedFlag))
		{
			Redraw();
		}
	}
}

/******************************************************************************
 HandleMouseUp

 ******************************************************************************/

void
JXTabGroup::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragAction == kClose)
	{
		if (button == kJXLeftButton && itsClosePushedFlag &&
			OKToDeleteTab(itsMouseIndex))
		{
			DeleteTab(itsMouseIndex);
		}
	}

	itsScrollUpPushedFlag = itsScrollDownPushedFlag = itsClosePushedFlag = false;
	Refresh();
}

/******************************************************************************
 OKToDeleteTab (virtual protected)

	Returns false if base class should not remove the tab.

 ******************************************************************************/

bool
JXTabGroup::OKToDeleteTab
	(
	const JIndex index
	)
{
	return true;
}

/******************************************************************************
 FindTab (protected)

 ******************************************************************************/

bool
JXTabGroup::FindTab
	(
	const JPoint&	pt,
	JIndex*			index,
	JRect*			rect
	)
	const
{
	const JSize count = itsTabRects->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		*rect = itsTabRects->GetElement(i);
		if (rect->Contains(pt))
		{
			*index = itsFirstDrawIndex + i-1;
			return true;
		}
	}

	*index = 0;
	rect->Set(0,0,0,0);
	return false;
}

/******************************************************************************
 ScrollWait

 ******************************************************************************/

void
JXTabGroup::ScrollWait
	(
	const JFloat delta
	)
	const
{
	GetWindow()->Update();
	JWait(delta);
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Mouseover a tab displays the tab.

 ******************************************************************************/

bool
JXTabGroup::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	*action = GetDNDManager()->GetDNDActionPrivateXAtom();
	return true;
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
JXTabGroup::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	JIndex i;
	JRect r;
	if (FindTab(pt, &i, &r))
	{
		ShowTab(i);
	}
}

/******************************************************************************
 HandleDNDScroll (virtual protected)

	This is called while the mouse is inside the widget, even if the widget
	does not currently accept the drop, because it might accept it after it
	is scrolled.

 ******************************************************************************/

void
JXTabGroup::HandleDNDScroll
	(
	const JPoint&			pt,
	const JXMouseButton		scrollButton,
	const JXKeyModifiers&	modifiers
	)
{
	if (scrollButton != 0)
	{
		ScrollForWheel(scrollButton, modifiers);
	}
}

/******************************************************************************
 ScrollForWheel (protected)

 ******************************************************************************/

bool
JXTabGroup::ScrollForWheel
	(
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers
	)
{
	if ((button == kJXButton4 || button == kJXButton6) &&
		itsCanScrollUpFlag && itsFirstDrawIndex > 1)
	{
		itsFirstDrawIndex--;
		Refresh();
		return true;
	}
	else if ((button == kJXButton5 || button == kJXButton7) &&
			 itsCanScrollDownFlag && itsFirstDrawIndex < GetTabCount())
	{
		itsFirstDrawIndex++;
		Refresh();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTabGroup::Activate()
{
	JXWidget::Activate();

	JIndex i;
	if (WouldBeActive() && GetTabCount() > 0 && !GetCurrentTabIndex(&i))
	{
		ShowTab(1);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXTabGroup::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsContextMenu && message.Is(JXTextMenu::kNeedsUpdate))
	{
		UpdateContextMenu();
	}
	else if (sender == itsContextMenu && message.Is(JXTextMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleContextMenu(selection->GetIndex());
	}

	else
	{
		JXWidget::Receive(sender, message);
	}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

void
JXTabGroup::CreateContextMenu()
{
	if (itsContextMenu == nullptr)
	{
		itsContextMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != nullptr );
		itsContextMenu->SetMenuItems(kContextMenuStr, "JXTabGroup");
		itsContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsContextMenu->SetToHiddenPopupMenu();
		ListenTo(itsContextMenu);
	}
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
JXTabGroup::UpdateContextMenu()
{
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
JXTabGroup::HandleContextMenu
	(
	const JIndex index
	)
{
	if (index == kContextTopCmd)
	{
		SetTabEdge(kTop);
	}
	else if (index == kContextBottomCmd)
	{
		SetTabEdge(kBottom);
	}
	else if (index == kContextLeftCmd)
	{
		SetTabEdge(kLeft);
	}
	else if (index == kContextRightCmd)
	{
		SetTabEdge(kRight);
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXTabGroup::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
	{
		long edge;
		input >> edge;
		SetTabEdge((Edge) edge);
	}

	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 SkipSetup (static)

 ******************************************************************************/

void
JXTabGroup::SkipSetup
	(
	std::istream& input
	)
{
	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXTabGroup::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << (long) itsEdge;
	output << kSetupDataEndDelimiter;
}

/******************************************************************************
 NeedsInternalFTC (virtual protected)

	Return true if the contents are a set of widgets that need to expand.

 ******************************************************************************/

bool
JXTabGroup::NeedsInternalFTC()
	const
{
	return true;
}
