/******************************************************************************
 JXMenu.cpp

	When the menu is pulled down, the requested items are initially disabled
	(see UpdateAction).  The default action is to disable all items that do
	not have submenus (kDisableSingles).

	All items are unchecked because the menu's owner should store this
	information anyway, and it saves a lot of work to not have to enforce
	one selection per radio group.  The client should be storing only a single
	value for each radio group anyway, so it will be taken care of automatically.

	If the menu is a PopupChoice, then it assumes that the entire menu is a
	radio group so that only one item can be selected at a time.  The owner
	of the menu remains responsible for tracking the selection.  We simply
	include the text of the selected item in the menu title as a convenience.
	Note that SetPopupChoice() does not need to be called when ItemSelected
	is broadcast because we take care of it automatically in this case.

	Items are not required to have ID's, but if they do, they should be
	unique (like JBroadcaster messages) so each object can find its own
	items.  The exception is an item that performs a standard action like
	Copy and can therefore be shared by all objects.  The standard ID's
	are stored in jXActions.h

	BASE CLASS = JXWidget

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "JXMenu.h"
#include "JXMenuData.h"
#include "JXMenuDirector.h"
#include "JXMenuTable.h"
#include "JXMenuBar.h"
#include "JXMenuManager.h"

#include "JXDisplay.h"
#include "JXWindowDirector.h"
#include "JXWindowPainter.h"
#include "JXWindow.h"
#include "JXImage.h"
#include "JXColorManager.h"
#include "jXPainterUtil.h"
#include "jXGlobals.h"

#include <JFontManager.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jMath.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

JXMenu::Style JXMenu::theDefaultStyle = JXMenu::kMacintoshStyle;
JXMenu::Style JXMenu::theDisplayStyle = JXMenu::kWindowsStyle;

const JCoordinate kImageTextBufferWidth = 4;

// information for drawing arrow when used as popup menu

const JCoordinate kTotalArrowWidth = 26;
const JCoordinate kArrowWidth      = 14;
const JCoordinate kArrowHalfHeight = 3;

// JBroadcaster message types

const JUtf8Byte* JXMenu::kNeedsUpdate  = "NeedsUpdate::JXMenu";
const JUtf8Byte* JXMenu::kItemSelected = "ItemSelected::JXMenu";

/******************************************************************************
 Constructor

	*** derived class must call SetItemData()

 ******************************************************************************/

JXMenu::JXMenu
	(
	const JString&		title,
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
	itsTitleFont(JFontManager::GetDefaultFont())
{
	JXMenuX(title, nullptr, true);
}

JXMenu::JXMenu
	(
	JXImage*			image,
	const bool		menuOwnsImage,
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
	itsTitleFont(JFontManager::GetDefaultFont())
{
	JXMenuX(JString::empty, image, menuOwnsImage);
}

JXMenu::JXMenu
	(
	const JString&		title,
	JXImage*			image,
	const bool		menuOwnsImage,
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
	itsTitleFont(JFontManager::GetDefaultFont())
{
	JXMenuX(title, image, menuOwnsImage);
}

JXMenu::JXMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXWidget(enclosure, kFixedLeft, kFixedTop, 0,0, 10,10),
	itsTitleFont(JFontManager::GetDefaultFont())
{
	JXMenuX(JString::empty, nullptr, true);
	owner->AttachSubmenu(itemIndex, this);
}

// private

void
JXMenu::JXMenuX
	(
	const JString&		title,
	JXImage*			image,
	const bool		menuOwnsImage
	)
{
	const JSize lineHeight = itsTitleFont.GetLineHeight(GetFontManager());
	itsTitlePadding.Set(lineHeight/2, lineHeight/4);

	itsTitleImage         = nullptr;
	itsOwnsTitleImageFlag = false;
	itsShortcuts          = nullptr;
	itsULIndex            = 0;

	itsBaseItemData = nullptr;
	itsMenuBar      = nullptr;
	itsOwner        = nullptr;
	itsUpdateAction = kDisableSingles;

	itsArrowPosition         = kArrowAtRight;
	itsArrowDirection        = kArrowPointsDown;
	itsIsPopupChoiceFlag     = false;
	itsIsHiddenPopupMenuFlag = false;
	itsMenuDirector          = nullptr;

	itsWaitingForFTCFlag = true;

	SetTitle(title, image, menuOwnsImage);

	itsShouldBeActiveFlag = WouldBeActive();
	itsUpdateSBAFlag      = false;
	Deactivate();
	itsUpdateSBAFlag      = true;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMenu::~JXMenu()
{
	assert( itsMenuDirector == nullptr );

	if (itsMenuBar != nullptr)
		{
		assert( itsOwner == nullptr );
		itsMenuBar->RemoveMenu(this);
		}
	else if (itsOwner != nullptr)
		{
		assert( itsMenuBar == nullptr );
		itsOwner->RemoveSubmenu(this);
		}

	jdelete itsShortcuts;

	if (itsOwnsTitleImageFlag)
		{
		jdelete itsTitleImage;
		}
}

/******************************************************************************
 SetBaseItemData (protected)

	*** This must be called in the derived class constructor after the
		derived class of JXMenuData has been created.

 ******************************************************************************/

void
JXMenu::SetBaseItemData
	(
	JXMenuData* baseItemData
	)
{
	assert( itsBaseItemData == nullptr && baseItemData != nullptr );

	itsBaseItemData = baseItemData;
	ListenTo(itsBaseItemData->GetList());
}

/******************************************************************************
 ClearBaseItemData (protected)

	*** This must be called in the derived class destructor before the
		JXMenuData object is deleted.

 ******************************************************************************/

void
JXMenu::ClearBaseItemData()
{
	assert( itsBaseItemData != nullptr );

	StopListening(itsBaseItemData->GetList());
	itsBaseItemData = nullptr;
}

/******************************************************************************
 SetTitle

	If title includes "%h", everything following this is passed to
	SetShortcuts().

	It is generally a bad idea to use both text and an image in the title
	of a menu in a menu bar, because it will look like two separate menus.
	Having both text and an image is most appropriate for popup menus
	elsewhere in a window.

 ******************************************************************************/

void
JXMenu::SetTitle
	(
	const JString&	title,
	JXImage*		image,
	const bool	menuOwnsImage
	)
{
	if (title.IsEmpty())
		{
		itsTitle.Clear();
		itsULIndex = 0;
		}
	else
		{
		itsTitle = title;

		JStringIterator iter(&itsTitle);
		if (iter.Next("%h"))
			{
			iter.RemoveLastMatch();
			iter.BeginMatch();
			iter.MoveTo(kJIteratorStartAtEnd, 0);

			JString shortcuts = iter.FinishMatch().GetString();
			shortcuts.TrimWhitespace();
			SetShortcuts(shortcuts);

			iter.RemoveLastMatch();
			itsTitle.TrimWhitespace();	// invalidates iterator
			}
		else
			{
			iter.Invalidate();
			itsULIndex = JXWindow::GetULShortcutIndex(itsTitle, itsShortcuts);
			}
		}

	if (itsOwnsTitleImageFlag)
		{
		jdelete itsTitleImage;
		}
	itsTitleImage         = image;
	itsOwnsTitleImageFlag = menuOwnsImage;

	UpdateTitleGeometry();
}

/******************************************************************************
 SetTitle...

	Convenience functions.

 ******************************************************************************/

void
JXMenu::SetTitleText
	(
	const JString& title
	)
{
	const bool saveOwn = itsOwnsTitleImageFlag;
	itsOwnsTitleImageFlag  = false;
	SetTitle(title, itsTitleImage, saveOwn);
}

void
JXMenu::SetTitleImage
	(
	JXImage*		image,
	const bool	menuOwnsImage
	)
{
	const JString title = itsTitle;		// make copy so don't invalidate char*
	SetTitle(title, image, menuOwnsImage);
}

/******************************************************************************
 UpdateTitleGeometry (private)

 ******************************************************************************/

void
JXMenu::UpdateTitleGeometry()
{
	JCoordinate w = 0;
	if (!itsTitle.IsEmpty())
		{
		w += itsTitleFont.GetStringWidth(GetFontManager(), itsTitle);
		}
	if (itsTitleImage != nullptr)
		{
		if (w > 0)
			{
			w += kImageTextBufferWidth;
			}
		w += itsTitleImage->GetWidth();
		}
	if (w > 0)
		{
		w += 2*itsTitlePadding.x;
		}
	AdjustAppearance(w);
	Refresh();
}

/******************************************************************************
 SetShortcuts

 ******************************************************************************/

void
JXMenu::SetShortcuts
	(
	const JString& list
	)
{
	if (theDisplayStyle == kMacintoshStyle)
		{
		return;
		}

	JXWindow* w = GetWindow();
	w->ClearShortcuts(this);
	w->InstallShortcuts(this, list);

	#define LabelVarName	itsTitle
	#include "JXUpdateShortcutIndex.th"
	#undef LabelVarName

	Refresh();
}

/******************************************************************************
 SetTitleFontName

 ******************************************************************************/

void
JXMenu::SetTitleFontName
	(
	const JString& fontName
	)
{
	itsTitleFont.SetName(fontName);

	if (!itsTitle.IsEmpty())
		{
		UpdateTitleGeometry();
		}
}

/******************************************************************************
 SetTitleFontSize

 ******************************************************************************/

void
JXMenu::SetTitleFontSize
	(
	const JSize size
	)
{
	itsTitleFont.SetSize(size);

	if (!itsTitle.IsEmpty())
		{
		UpdateTitleGeometry();
		}
}

/******************************************************************************
 SetTitleFontStyle

 ******************************************************************************/

void
JXMenu::SetTitleFontStyle
	(
	const JFontStyle& style
	)
{
	itsTitleFont.SetStyle(style);

	if (!itsTitle.IsEmpty())
		{
		UpdateTitleGeometry();
		}
}

/******************************************************************************
 SetToPopupChoice (virtual)

 ******************************************************************************/

void
JXMenu::SetToPopupChoice
	(
	const bool	isPopup,
	const JIndex	initialChoice
	)
{
	itsIsPopupChoiceFlag = isPopup;
	if (itsIsPopupChoiceFlag)
		{
		AdjustPopupChoiceTitle(initialChoice);
		}
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

bool
JXMenu::IsEmpty()
	const
{
	return itsBaseItemData->IsEmpty();
}

/******************************************************************************
 GetItemCount

 ******************************************************************************/

JSize
JXMenu::GetItemCount()
	const
{
	return itsBaseItemData->GetElementCount();
}

/******************************************************************************
 RemoveAllItems

 ******************************************************************************/

void
JXMenu::RemoveAllItems()
{
	itsBaseItemData->DeleteAll();
}

/******************************************************************************
 RemoveItem

 ******************************************************************************/

void
JXMenu::RemoveItem
	(
	const JIndex index
	)
{
	itsBaseItemData->DeleteItem(index);
}

/******************************************************************************
 GetItemShortcuts

 ******************************************************************************/

bool
JXMenu::GetItemShortcuts
	(
	const JIndex	index,
	JString*		shortcuts
	)
	const
{
	const JString* s;
	if (itsBaseItemData->GetItemShortcuts(index, &s))
		{
		*shortcuts = *s;
		return true;
		}
	else
		{
		shortcuts->Clear();
		return false;
		}
}

/******************************************************************************
 SetItemShortcuts

 ******************************************************************************/

void
JXMenu::SetItemShortcuts
	(
	const JIndex	index,
	const JString&	shortcuts
	)
{
	itsBaseItemData->SetItemShortcuts(index, shortcuts);
}

/******************************************************************************
 GetItemID

	Returns true if the item has an id.

 ******************************************************************************/

bool
JXMenu::GetItemID
	(
	const JIndex	index,
	const JString**	id
	)
	const
{
	return itsBaseItemData->GetItemID(index, id);
}

/******************************************************************************
 SetItemID

 ******************************************************************************/

void
JXMenu::SetItemID
	(
	const JIndex	index,
	const JString&	id
	)
{
	itsBaseItemData->SetItemID(index, id);
}

/******************************************************************************
 ItemIDToIndex

 ******************************************************************************/

bool
JXMenu::ItemIDToIndex
	(
	const JString&	targetID,
	JIndex*			index
	)
	const
{
	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* id;
		if (GetItemID(i, &id) && *id == targetID)
			{
			*index = i;
			return true;
			}
		}

	*index = 0;
	return false;
}

/******************************************************************************
 IsEnabled

 ******************************************************************************/

bool
JXMenu::IsEnabled
	(
	const JIndex index
	)
	const
{
	return itsBaseItemData->IsEnabled(index);
}

/******************************************************************************
 EnableItem

 ******************************************************************************/

void
JXMenu::EnableItem
	(
	const JIndex index
	)
{
	itsBaseItemData->EnableItem(index);
}

/******************************************************************************
 EnableAll

 ******************************************************************************/

void
JXMenu::EnableAll()
{
	itsBaseItemData->EnableAll();
}

/******************************************************************************
 DisableItem

 ******************************************************************************/

void
JXMenu::DisableItem
	(
	const JIndex index
	)
{
	itsBaseItemData->DisableItem(index);
}

/******************************************************************************
 DisableAll

 ******************************************************************************/

void
JXMenu::DisableAll()
{
	itsBaseItemData->DisableAll();
}

/******************************************************************************
 SetItemEnable (virtual protected)

 ******************************************************************************/

void
JXMenu::SetItemEnable
	(
	const JIndex	index,
	const bool	enabled
	)
{
	itsBaseItemData->SetItemEnable(index, enabled);
}

/******************************************************************************
 IsChecked

 ******************************************************************************/

bool
JXMenu::IsChecked
	(
	const JIndex index
	)
	const
{
	return itsBaseItemData->IsChecked(index);
}

/******************************************************************************
 CheckItem

 ******************************************************************************/

void
JXMenu::CheckItem
	(
	const JIndex index
	)
{
	itsBaseItemData->CheckItem(index);
}

/******************************************************************************
 AttachSubmenu

	Deletes any old menu that was attached and attaches the new one.

 ******************************************************************************/

void
JXMenu::AttachSubmenu
	(
	const JIndex	index,
	JXMenu*			submenu
	)
{
	itsBaseItemData->AttachSubmenu(index, submenu);
	submenu->SetOwner(this);
	submenu->Hide();
}

/******************************************************************************
 RemoveSubmenu

	Returns true if there was a submenu at the given index.
	The caller now owns the menu and is responsible for deleting it.

 ******************************************************************************/

bool
JXMenu::RemoveSubmenu
	(
	const JIndex	index,
	JXMenu**		theMenu
	)
{
	return itsBaseItemData->RemoveSubmenu(index, theMenu);
}

// private

void
JXMenu::RemoveSubmenu
	(
	JXMenu* theMenu
	)
{
	itsBaseItemData->RemoveSubmenu(theMenu);
}

/******************************************************************************
 DeleteSubmenu

 ******************************************************************************/

void
JXMenu::DeleteSubmenu
	(
	const JIndex index
	)
{
	itsBaseItemData->DeleteSubmenu(index);
}

/******************************************************************************
 GetSubmenu

 ******************************************************************************/

bool
JXMenu::GetSubmenu
	(
	const JIndex	index,
	const JXMenu**	menu
	)
	const
{
	return itsBaseItemData->GetSubmenu(index, const_cast<JXMenu**>(menu));
}

/******************************************************************************
 GetMenuBar

	Returns true if we are part of a menu bar.

 ******************************************************************************/

bool
JXMenu::GetMenuBar
	(
	JXMenuBar** menuBar
	)
	const
{
	JXMenu* topMenu = GetTopLevelMenu();
	*menuBar = topMenu->itsMenuBar;
	return *menuBar != nullptr;
}

/******************************************************************************
 GetTopLevelMenu

	Return the top level menu in our tree.

 ******************************************************************************/

JXMenu*
JXMenu::GetTopLevelMenu()
	const
{
	JXMenu* topMenu = const_cast<JXMenu*>(this);
	while (topMenu->itsOwner != nullptr)
		{
		topMenu = topMenu->itsOwner;
		}
	return topMenu;
}

/******************************************************************************
 SetMenuBar (private for JXMenuBar)

 ******************************************************************************/

void
JXMenu::SetMenuBar
	(
	JXMenuBar* bar
	)
{
	if (itsMenuBar == bar)
		{
		return;
		}
	else if (itsMenuBar != nullptr)
		{
		assert( itsOwner == nullptr );
		itsMenuBar->RemoveMenu(this);
		}
	else if (itsOwner != nullptr)
		{
		assert( itsMenuBar == nullptr );
		itsOwner->RemoveSubmenu(this);
		}

	itsMenuBar = bar;
	itsOwner   = nullptr;

	AdjustAppearance();
}

/******************************************************************************
 SetOwner (private)

 ******************************************************************************/

void
JXMenu::SetOwner
	(
	JXMenu* owner
	)
{
	if (itsOwner == owner)
		{
		return;
		}
	else if (itsOwner != nullptr)
		{
		assert( itsMenuBar == nullptr );
		itsOwner->RemoveSubmenu(this);
		}
	else if (itsMenuBar != nullptr)
		{
		assert( itsOwner == nullptr );
		itsMenuBar->RemoveMenu(this);
		}

	itsMenuBar = nullptr;
	itsOwner   = owner;

	AdjustAppearance();
}

/******************************************************************************
 AdjustAppearance (private)

 ******************************************************************************/

inline void
JXMenu::AdjustAppearance()
{
	AdjustAppearance(itsMinWidth);
}

/******************************************************************************
 AdjustAppearance (private)

 ******************************************************************************/

void
JXMenu::AdjustAppearance
	(
	const JCoordinate minWidth
	)
{
	// use the same cursor for consistency

	if (itsOwner != nullptr)
		{
		SetDefaultCursor(itsOwner->GetDefaultCursor());
		}
	else if (itsMenuBar != nullptr)
		{
		SetDefaultCursor(itsMenuBar->GetDefaultCursor());
		}

	// adjust width

	itsMinWidth = minWidth;

	JCoordinate w = minWidth;
	if (itsOwner == nullptr && itsMenuBar == nullptr)
		{
		SetBorderWidth(kJXDefaultBorderWidth);
		if (itsWaitingForFTCFlag)
			{
			return;
			}

		w += kTotalArrowWidth + 2*kJXDefaultBorderWidth;
		}
	else
		{
		SetBorderWidth(0);
		}

	if (w <= 0)
		{
		w = 2*itsTitlePadding.x;
		}

	const JCoordinate dw = w - GetFrameWidth();
	if (dw != 0)
		{
		AdjustSize(dw, 0);
		if (itsMenuBar != nullptr)
			{
			itsMenuBar->MenuWidthChanged(this, dw);
			}
		}
}

/******************************************************************************
 PrepareToOpenMenu (protected)

	Returns false if the menu is completely disabled.

 ******************************************************************************/

bool
JXMenu::PrepareToOpenMenu
	(
	const bool shortcut
	)
{
	if (itsOwner != nullptr && !itsOwner->IsOpen())	// parent may have effect on child
		{
		itsOwner->PrepareToOpenMenu(shortcut);
		}

	if ((itsOwner == nullptr &&			// active status of sub-menus depends on menu item active status
		 !IsActive()) ||
		(itsOwner == nullptr &&			// visibility is irrelevant for sub-menus
		 !itsIsHiddenPopupMenuFlag &&	// and hidden popups
		 !IsVisible()))
		{
		return false;
		}

	// disable the requested items, uncheck all items

	itsBaseItemData->PrepareToOpenMenu(itsUpdateAction);

	// let owner update us as appropriate

	Broadcast(NeedsUpdate(shortcut));
	return IsActive() && GetItemCount() > 0;
}

/******************************************************************************
 Open (private)

	Callers can provide two different points where the menu could
	reasonably be placed (left and right for submenus).

	Returns true if successful.

 ******************************************************************************/

bool
JXMenu::Open
	(
	const JPoint& userLeftPtR,
	const JPoint& userRightPtR
	)
{
	JXMenuManager* menuManager = GetMenuManager();
	if (itsOwner == nullptr)
		{
		menuManager->CloseCurrentMenus();
		}
	assert( itsMenuDirector == nullptr );

	if (!PrepareToOpenMenu(false))
		{
		return false;
		}
	assert( !IsEmpty() );

	// create window to display menu

	JPoint leftPtR, rightPtR;
	JCoordinate frameHeight = 0;
	if (itsOwner == nullptr)
		{
		// place below widget frame

		JXWindow* w  = GetWindow();
		JRect frameG = GetFrameGlobal();
		leftPtR      = w->GlobalToRoot(frameG.bottomRight());
		rightPtR     = w->GlobalToRoot(frameG.bottomLeft());

		if (itsIsHiddenPopupMenuFlag)
			{
			leftPtR = rightPtR;
			leftPtR.x++;	// make sure mouse ends up inside menu window, so menu stays open
			}
		else
			{
			frameHeight = frameG.height();
			}
		}
	else
		{
		// use suggested points

		leftPtR  = userLeftPtR;
		rightPtR = userRightPtR;
		}

	itsMenuDirector = CreateMenuWindow(GetWindow()->GetDirector());
	itsMenuDirector->BuildWindow(leftPtR, rightPtR, frameHeight);

	if (itsIsHiddenPopupMenuFlag)
		{
		(itsMenuDirector->GetWindow())->SetWMWindowType(JXWindow::kWMPopupMenuType);
		}

	itsMenuDirector->Activate();
	itsMenuDirector->GrabKeyboard();
	menuManager->MenuOpened(this, itsMenuDirector->GetWindow());
	Redraw();
	return true;
}

/******************************************************************************
 Close (private)

 ******************************************************************************/

void
JXMenu::Close()
{
	assert( itsMenuDirector != nullptr );

	GetMenuManager()->MenuClosed(this);

	const bool ok = itsMenuDirector->Close();
	assert( ok );
	itsMenuDirector = nullptr;

	Redraw();

	if (itsOwner == nullptr)
		{
		GetWindow()->RequestFocus();
		}
}

/******************************************************************************
 BroadcastSelection (protected)

 ******************************************************************************/

void
JXMenu::BroadcastSelection
	(
	const JIndex	itemIndex,
	const bool	fromShortcut
	)
{
	// close all menus

	GetMenuManager()->CloseCurrentMenus();

	// update the menu title to show the new selection

	if (itsIsPopupChoiceFlag)
		{
		AdjustPopupChoiceTitle(itemIndex);
		}

	// broadcast the message

	Broadcast(ItemSelected(itemIndex, fromShortcut));		// can delete us
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXMenu::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect bounds = GetBounds();

	const JCoordinate borderWidth = GetBorderWidth();
	if (itsMenuDirector != nullptr && borderWidth == 0)
		{
		JXDrawUpFrame(p, bounds, kJXDefaultBorderWidth);
		}

	JRect r = bounds;
	if (borderWidth > 0)
		{
		JRect ra  = bounds;
		ra.top    = bounds.ycenter() - kArrowHalfHeight;
		ra.bottom = ra.top + 2*kArrowHalfHeight;

		if (itsArrowPosition == kArrowAtRight)
			{
			ra.left  = bounds.right - (kTotalArrowWidth + kArrowWidth)/2;
			ra.right = ra.left + kArrowWidth;
			r.right -= kTotalArrowWidth;
			ra.left++;
			}
		else
			{
			assert( itsArrowPosition == kArrowAtLeft );
			ra.right = bounds.left + (kTotalArrowWidth + kArrowWidth)/2;
			ra.left  = ra.right - kArrowWidth;
			r.left  += kTotalArrowWidth;
			ra.right--;
			}

		const JColorID colorIndex =
			IsActive() ? JColorManager::GetGrayColor(40) :
						 JColorManager::GetInactiveLabelColor();
		if (itsArrowDirection == kArrowPointsDown)
			{
			JXFillArrowDown(p, ra, colorIndex);
			}
		else
			{
			ra.top--;
			JXFillArrowUp(p, ra, colorIndex);
			}
		}

	p.SetClipRect(r);

	r.left += itsTitlePadding.x;
	if (itsTitleImage != nullptr)
		{
		JRect ir = r;
		ir.right = ir.left + itsTitleImage->GetWidth();
		p.Image(*itsTitleImage, itsTitleImage->GetBounds(), ir);
		r.left = ir.right + kImageTextBufferWidth;
		}

	if (!itsTitle.IsEmpty())
		{
		if (IsActive())
			{
			p.SetFont(itsTitleFont);
			}
		else
			{
			JFont f = itsTitleFont;
			f.SetColor(JColorManager::GetInactiveLabelColor());
			p.SetFont(f);
			}

		p.String(r.left, r.top, itsTitle, itsULIndex,
				 r.width(), JPainter::kHAlignLeft,
				 r.height(), JPainter::kVAlignCenter);
		}

	p.ResetClipRect();
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXMenu::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	const JCoordinate borderWidth = GetBorderWidth();
	if (borderWidth > 0 && IsActive())
		{
		JXDrawUpFrame(p, frame, borderWidth);
		}
	else if (borderWidth > 0)
		{
		p.SetLineWidth(borderWidth);
		p.SetPenColor(JColorManager::GetInactiveLabelColor());
		p.RectInside(frame);
		}
}

/******************************************************************************
 PopUp

	Call this to make the menu pop open at the specified point inside
	mouseOwner.  This must only be called from mouseOwner's HandleMouseDown()
	or HandleMouseDrag().  If this function is successful, you will already
	have received HandleMouseUp() by the time it returns.  It will return true
	so you know that it succeeded and that you should return immediately.

	pt must be in the bounds coordinates of mouseOwner.  In addition, the
	menu must not be visible, because this would confuse the user.  We also
	require that the menu is not in a menu bar and is not a sub-menu.

	Try to avoid using this feature since it will usually not be obvious
	to the user that a menu can be popped up at all.

 ******************************************************************************/

bool
JXMenu::PopUp
	(
	JXContainer*			mouseOwner,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	assert( itsIsHiddenPopupMenuFlag );

	if (itsMenuBar == nullptr && itsOwner == nullptr &&
		!IsVisible() && !buttonStates.AllOff() &&
		itsMenuDirector == nullptr)
		{
		Place(pt.x, pt.y - GetFrameHeight());
		if (Open())
			{
			return GetDisplay()->SwitchDrag(mouseOwner, pt, buttonStates, modifiers,
											  itsMenuDirector->GetMenuTable());
			}
		}

	return false;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXMenu::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton && itsMenuDirector == nullptr && Open())
		{
		const bool ok =
			GetDisplay()->SwitchDrag(this, pt, buttonStates, modifiers,
									   itsMenuDirector->GetMenuTable());
		if (!ok && IsOpen())	// SwitchDrag() can trigger Close()
			{
			Close();
			}
		}
}

/******************************************************************************
 HandleShortcut

	All shortcuts open the menu.

 ******************************************************************************/

void
JXMenu::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsMenuDirector == nullptr && Open())
		{
		itsMenuDirector->GrabPointer();
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXMenu::Activate()
{
	itsShouldBeActiveFlag = true;
	if ((itsBaseItemData->GetList())->IsEmpty())
		{
		return;
		}

	const bool wasActive = IsActive();
	JXWidget::Activate();
	if (!wasActive && IsActive())
		{
		PrivateActivate();
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXMenu::Deactivate()
{
	if (itsUpdateSBAFlag)
		{
		itsShouldBeActiveFlag = false;
		}

	const bool wasActive = IsActive();
	JXWidget::Deactivate();
	if (wasActive && !IsActive())
		{
		PrivateDeactivate();
		}
}

/******************************************************************************
 Suspend (virtual)

	We do not support suspending submenus.  If we suspend a submenu, it
	deactivates the menu, so Resume() will still think the menu is not
	active.

 ******************************************************************************/

void
JXMenu::Suspend()
{
	const bool wasActive = IsActive();
	JXWidget::Suspend();
	if (itsOwner == nullptr && wasActive && !IsActive())
		{
		PrivateDeactivate();
		}
}

/******************************************************************************
 Resume (virtual)

	We have to process resume because if a window is created while the
	application is suspended, the menu will initially be disabled.

 ******************************************************************************/

void
JXMenu::Resume()
{
	const bool wasActive = IsActive();
	JXWidget::Resume();
	if (!wasActive && IsActive())
		{
		PrivateActivate();
		}
}

/******************************************************************************
 PrivateActivate (private)

 ******************************************************************************/

void
JXMenu::PrivateActivate()
{
	JIndex ownerItemIndex;
	if (itsOwner != nullptr &&
		!(itsBaseItemData->GetList())->IsEmpty() &&
		itsOwner->itsBaseItemData->FindSubmenu(this, &ownerItemIndex))
		{
		itsOwner->EnableItem(ownerItemIndex);
		}
}

/******************************************************************************
 PrivateDeactivate (private)

 ******************************************************************************/

void
JXMenu::PrivateDeactivate()
{
	JIndex ownerItemIndex;
	if (itsOwner != nullptr &&
		itsOwner->itsBaseItemData->FindSubmenu(this, &ownerItemIndex))
		{
		itsOwner->DisableItem(ownerItemIndex);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	const JCollection* array = itsBaseItemData->GetList();

	if (sender == const_cast<JCollection*>(array))
		{
		if (array->IsEmpty() && IsActive())
			{
			itsUpdateSBAFlag = false;
			Deactivate();
			itsUpdateSBAFlag = true;
			}
		else if (itsShouldBeActiveFlag && !IsActive())
			{
			Activate();
			}
		}
	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustNMShortcutModifier (static)

 ******************************************************************************/

JXModifierKey
JXMenu::AdjustNMShortcutModifier
	(
	const JXModifierKey key
	)
{
	if ((theDisplayStyle == kMacintoshStyle && theDefaultStyle == kWindowsStyle) ||
		(theDisplayStyle == kWindowsStyle   && theDefaultStyle == kMacintoshStyle))
		{
		if (key == kJXControlKeyIndex)
			{
			return kJXMetaKeyIndex;
			}
		else if (key == kJXMetaKeyIndex)
			{
			return kJXControlKeyIndex;
			}
		}

	return key;
}

/******************************************************************************
 Menu ID routine (virtual)

 ******************************************************************************/

bool
JXMenu::IsMenu()
	const
{
	return true;
}

/******************************************************************************
 IncludeInFTC (virtual protected)

 ******************************************************************************/

bool
JXMenu::IncludeInFTC()
	const
{
	return !itsIsHiddenPopupMenuFlag && itsOwner == nullptr &&
				JXContainer::IncludeInFTC();
}

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

 ******************************************************************************/

JCoordinate
JXMenu::GetFTCMinContentSize
	(
	const bool horizontal
	)
	const
{
	const_cast<JXMenu*>(this)->itsWaitingForFTCFlag = false;

	if (horizontal && IsHiddenPopupMenu())
		{
		return GetApertureWidth();
		}

	JCoordinate widgetBorderWidth = GetBorderWidth(),
				borderWidth = 0,
				arrowWidth  = 0;
	if (widgetBorderWidth == 0)
		{
		borderWidth = kJXDefaultBorderWidth;
		}
	else if (widgetBorderWidth > 0)
		{
		arrowWidth = kTotalArrowWidth;
		}

	if (itsTitle.IsEmpty())
		{
		const JRect apG = GetApertureGlobal();
		return horizontal ? apG.width() : apG.height();
		}
	else if (horizontal)
		{
		const_cast<JXMenu*>(this)->itsWaitingForFTCFlag = true;
		const JSize tw = IsPopupChoice() ?
			GetMaxPopupChoiceTitleWidth() :
			itsTitleFont.GetStringWidth(GetFontManager(), itsTitle);
		const_cast<JXMenu*>(this)->itsWaitingForFTCFlag = false;

		return ((itsTitleImage != nullptr ? itsTitleImage->GetWidth() : 0) +
				(itsTitleImage != nullptr && tw > 0 ? kImageTextBufferWidth : 0) +
				(itsTitleImage != nullptr || tw > 0 ? 2*itsTitlePadding.x : 0) +
				tw +
				arrowWidth +
				2*borderWidth);
		}
	else
		{
		return (JMax(itsTitleImage != nullptr ? itsTitleImage->GetHeight() : 0,
					 (JCoordinate) itsTitleFont.GetLineHeight(GetFontManager())) +
				2*borderWidth +
				2*itsTitlePadding.y);
		}
}

/******************************************************************************
 GetMaxPopupChoiceTitleWidth (private)

 ******************************************************************************/

JSize
JXMenu::GetMaxPopupChoiceTitleWidth()
	const
{
	JXMenu* self = const_cast<JXMenu*>(this);	// conceptually const

	const JString origTitle      = itsTitle;
	JXImage* origTitleImage      = itsTitleImage;
	const bool origOwnsImage = itsOwnsTitleImageFlag;

	self->itsOwnsTitleImageFlag = false;		// don't delete the image, so we can restore it

	JSize max         = 0;
	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		self->AdjustPopupChoiceTitle(i);
		max = JMax(max, itsTitleFont.GetStringWidth(GetFontManager(), itsTitle));
		}

	self->SetTitle(origTitle, origTitleImage, origOwnsImage);
	return max;
}
