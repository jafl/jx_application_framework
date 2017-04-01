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

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXMenu.h>
#include <JXMenuData.h>
#include <JXMenuDirector.h>
#include <JXMenuTable.h>
#include <JXMenuBar.h>
#include <JXMenuManager.h>

#include <JXDisplay.h>
#include <JXWindowDirector.h>
#include <JXWindowPainter.h>
#include <JXWindow.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>

#include <JFontManager.h>
#include <jMath.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

JXMenu::Style JXMenu::theDefaultStyle = JXMenu::kMacintoshStyle;
JXMenu::Style JXMenu::theDisplayStyle = JXMenu::kWindowsStyle;

static const JCharacter* kMenuFontFileName  = "~/.jx/menu_font";
JBoolean JXMenu::theDefaultMenuFontInitFlag = kJFalse;
JString JXMenu::theDefaultFontName;
JSize JXMenu::theDefaultFontSize;

const JCoordinate kTitleExtraWidth      = 16;
const JCoordinate kImageTextBufferWidth = 4;

// information for drawing arrow when used as popup menu

const JCoordinate kTotalArrowWidth = 26;
const JCoordinate kArrowWidth      = 14;
const JCoordinate kArrowHalfHeight = 3;

// JBroadcaster message types

const JCharacter* JXMenu::kNeedsUpdate  = "NeedsUpdate::JXMenu";
const JCharacter* JXMenu::kItemSelected = "ItemSelected::JXMenu";

/******************************************************************************
 Constructor

	*** derived class must call SetItemData()

 ******************************************************************************/

JXMenu::JXMenu
	(
	const JCharacter*	title,
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
	itsTitleFont(GetFontManager()->GetDefaultFont())
{
	JXMenuX(title, NULL, kJTrue);
}

JXMenu::JXMenu
	(
	JXImage*			image,
	const JBoolean		menuOwnsImage,
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
	itsTitleFont(GetFontManager()->GetDefaultFont())
{
	JXMenuX("", image, menuOwnsImage);
}

JXMenu::JXMenu
	(
	const JCharacter*	title,
	JXImage*			image,
	const JBoolean		menuOwnsImage,
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
	itsTitleFont(GetFontManager()->GetDefaultFont())
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
	itsTitleFont(GetFontManager()->GetDefaultFont())
{
	JXMenuX("", NULL, kJTrue);
	owner->AttachSubmenu(itemIndex, this);
}

// private

void
JXMenu::JXMenuX
	(
	const JCharacter*	title,
	JXImage*			image,
	const JBoolean		menuOwnsImage
	)
{
	itsTitleImage         = NULL;
	itsOwnsTitleImageFlag = kJFalse;
	itsShortcuts          = NULL;
	itsULIndex            = 0;

	itsBaseItemData = NULL;
	itsMenuBar      = NULL;
	itsOwner        = NULL;
	itsUpdateAction = kDisableSingles;

	itsArrowPosition         = kArrowAtRight;
	itsArrowDirection        = kArrowPointsDown;
	itsIsPopupChoiceFlag     = kJFalse;
	itsIsHiddenPopupMenuFlag = kJFalse;
	itsMenuDirector          = NULL;

	SetTitle(title, image, menuOwnsImage);

	itsShouldBeActiveFlag = WouldBeActive();
	itsUpdateSBAFlag      = kJFalse;
	Deactivate();
	itsUpdateSBAFlag      = kJTrue;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMenu::~JXMenu()
{
	assert( itsMenuDirector == NULL );

	if (itsMenuBar != NULL)
		{
		assert( itsOwner == NULL );
		itsMenuBar->RemoveMenu(this);
		}
	else if (itsOwner != NULL)
		{
		assert( itsMenuBar == NULL );
		itsOwner->RemoveSubmenu(this);
		}

	jdelete itsShortcuts;

	if (itsOwnsTitleImageFlag)
		{
		jdelete itsTitleImage;
		}
}

/******************************************************************************
 Default menu font (static)

 ******************************************************************************/

const JCharacter*
JXMenu::GetDefaultFont
	(
	JSize* size
	)
{
	if (!theDefaultMenuFontInitFlag)
		{
		theDefaultMenuFontInitFlag = kJTrue;
		theDefaultFontName         = JGetDefaultFontName();
		theDefaultFontSize         = kJDefaultFontSize;

		JString fileName;
		if (JExpandHomeDirShortcut(kMenuFontFileName, &fileName))
			{
			std::ifstream input(fileName);
			input >> std::ws;
			JString name = JReadLine(input);
			name.TrimWhitespace();
			JSize size;
			input >> size;
			if (input.good())
				{
				theDefaultFontName = name;
				theDefaultFontSize = size;
				}
			}
		}

	*size = theDefaultFontSize;
	return theDefaultFontName;
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
	assert( itsBaseItemData == NULL && baseItemData != NULL );

	itsBaseItemData = baseItemData;
	ListenTo(itsBaseItemData->GetOrderedSet());
}

/******************************************************************************
 ClearBaseItemData (protected)

	*** This must be called in the derived class destructor before the
		JXMenuData object is deleted.

 ******************************************************************************/

void
JXMenu::ClearBaseItemData()
{
	assert( itsBaseItemData != NULL );

	StopListening(itsBaseItemData->GetOrderedSet());
	itsBaseItemData = NULL;
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
	const JCharacter*	title,
	JXImage*			image,
	const JBoolean		menuOwnsImage
	)
{
	if (JStringEmpty(title))
		{
		itsTitle.Clear();
		itsULIndex = 0;
		}
	else
		{
		itsTitle = title;

		JIndex i;
		if (itsTitle.LocateSubstring("%h", &i))
			{
			JString shortcuts(itsTitle, JIndexRange(i+2, itsTitle.GetLength()));
			shortcuts.TrimWhitespace();
			itsTitle.RemoveSubstring(i, itsTitle.GetLength());
			itsTitle.TrimWhitespace();
			SetShortcuts(shortcuts);
			}
		else
			{
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
	const JCharacter* title
	)
{
	const JBoolean saveOwn = itsOwnsTitleImageFlag;
	itsOwnsTitleImageFlag  = kJFalse;
	SetTitle(title, itsTitleImage, saveOwn);
}

void
JXMenu::SetTitleImage
	(
	JXImage*		image,
	const JBoolean	menuOwnsImage
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
		w += itsTitleFont.GetStringWidth(itsTitle);
		}
	if (itsTitleImage != NULL)
		{
		if (w > 0)
			{
			w += kImageTextBufferWidth;
			}
		w += itsTitleImage->GetWidth();
		}
	if (w > 0)
		{
		w += kTitleExtraWidth;
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
	const JCharacter* list
	)
{
	JXWindow* w = GetWindow();
	w->ClearShortcuts(this);
	w->InstallShortcuts(this, list);

	#define LabelVarName	itsTitle
	#include <JXUpdateShortcutIndex.th>
	#undef LabelVarName

	Refresh();
}

/******************************************************************************
 SetTitleFontName

 ******************************************************************************/

void
JXMenu::SetTitleFontName
	(
	const JCharacter* fontName
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
	const JBoolean	isPopup,
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

JBoolean
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

JBoolean
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
		return kJTrue;
		}
	else
		{
		shortcuts->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 SetItemShortcuts

 ******************************************************************************/

void
JXMenu::SetItemShortcuts
	(
	const JIndex		index,
	const JCharacter*	shortcuts
	)
{
	itsBaseItemData->SetItemShortcuts(index, shortcuts);
}

/******************************************************************************
 GetItemID

	Returns kJTrue if the item has an id.

 ******************************************************************************/

JBoolean
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
	const JIndex		index,
	const JCharacter*	id
	)
{
	itsBaseItemData->SetItemID(index, id);
}

/******************************************************************************
 ItemIDToIndex

 ******************************************************************************/

JBoolean
JXMenu::ItemIDToIndex
	(
	const JCharacter*	targetID,
	JIndex*				index
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
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 IsEnabled

 ******************************************************************************/

JBoolean
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
	const JBoolean	enabled
	)
{
	itsBaseItemData->SetItemEnable(index, enabled);
}

/******************************************************************************
 IsChecked

 ******************************************************************************/

JBoolean
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

	Returns kJTrue if there was a submenu at the given index.
	The caller now owns the menu and is responsible for deleting it.

 ******************************************************************************/

JBoolean
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

JBoolean
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

	Returns kJTrue if we are part of a menu bar.

 ******************************************************************************/

JBoolean
JXMenu::GetMenuBar
	(
	JXMenuBar** menuBar
	)
	const
{
	JXMenu* topMenu = GetTopLevelMenu();
	*menuBar = topMenu->itsMenuBar;
	return JConvertToBoolean( *menuBar != NULL );
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
	while (topMenu->itsOwner != NULL)
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
	else if (itsMenuBar != NULL)
		{
		assert( itsOwner == NULL );
		itsMenuBar->RemoveMenu(this);
		}
	else if (itsOwner != NULL)
		{
		assert( itsMenuBar == NULL );
		itsOwner->RemoveSubmenu(this);
		}

	itsMenuBar = bar;
	itsOwner   = NULL;

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
	else if (itsOwner != NULL)
		{
		assert( itsMenuBar == NULL );
		itsOwner->RemoveSubmenu(this);
		}
	else if (itsMenuBar != NULL)
		{
		assert( itsOwner == NULL );
		itsMenuBar->RemoveMenu(this);
		}

	itsMenuBar = NULL;
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
	itsMinWidth = minWidth;

	JCoordinate w = minWidth;
	if (itsOwner == NULL && itsMenuBar == NULL)
		{
		SetBorderWidth(kJXDefaultBorderWidth);
		w += kTotalArrowWidth + 2*kJXDefaultBorderWidth;
		}
	else
		{
		SetBorderWidth(0);
		}

	if (w <= 0)
		{
		w = kTitleExtraWidth;
		}

	const JCoordinate dw = w - GetFrameWidth();
	if (dw != 0)
		{
		AdjustSize(dw, 0);
		if (itsMenuBar != NULL)
			{
			itsMenuBar->MenuWidthChanged(this, dw);
			}
		}

	// use the same cursor for consistency

	if (itsOwner != NULL)
		{
		SetDefaultCursor(itsOwner->GetDefaultCursor());
		}
	else if (itsMenuBar != NULL)
		{
		SetDefaultCursor(itsMenuBar->GetDefaultCursor());
		}
}

/******************************************************************************
 PrepareToOpenMenu (protected)

	Returns kJFalse if the menu is completely disabled.

 ******************************************************************************/

JBoolean
JXMenu::PrepareToOpenMenu
	(
	const JBoolean shortcut
	)
{
	if (itsOwner != NULL && !itsOwner->IsOpen())	// parent may have effect on child
		{
		itsOwner->PrepareToOpenMenu(shortcut);
		}

	if ((itsOwner == NULL &&			// active status of sub-menus depends on menu item active status
		 !IsActive()) ||
		(itsOwner == NULL &&			// visibility is irrelevant for sub-menus
		 !itsIsHiddenPopupMenuFlag &&	// and hidden popups
		 !IsVisible()))
		{
		return kJFalse;
		}

	// disable the requested items, uncheck all items

	itsBaseItemData->PrepareToOpenMenu(itsUpdateAction);

	// let owner update us as appropriate

	Broadcast(NeedsUpdate(shortcut));
	return JI2B(IsActive() && GetItemCount() > 0);
}

/******************************************************************************
 Open (private)

	Callers can provide two different points where the menu could
	reasonably be placed (left and right for submenus).

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXMenu::Open
	(
	const JPoint& userLeftPtR,
	const JPoint& userRightPtR
	)
{
	JXMenuManager* menuManager = GetMenuManager();
	if (itsOwner == NULL)
		{
		menuManager->CloseCurrentMenus();
		}
	assert( itsMenuDirector == NULL );

	if (!PrepareToOpenMenu(kJFalse))
		{
		return kJFalse;
		}
	assert( !IsEmpty() );

	// create window to display menu

	JPoint leftPtR, rightPtR;
	JCoordinate frameHeight = 0;
	if (itsOwner == NULL)
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
	return kJTrue;
}

/******************************************************************************
 Close (private)

 ******************************************************************************/

void
JXMenu::Close()
{
	assert( itsMenuDirector != NULL );

	GetMenuManager()->MenuClosed(this);

	const JBoolean ok = itsMenuDirector->Close();
	assert( ok );
	itsMenuDirector = NULL;

	Redraw();

	if (itsOwner == NULL)
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
	const JBoolean	fromShortcut
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
	if (itsMenuDirector != NULL && borderWidth == 0)
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

		const JColorIndex colorIndex =
			IsActive() ? (p.GetColormap())->GetGrayColor(40) :
						 (p.GetColormap())->GetInactiveLabelColor();
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

	r.left += kTitleExtraWidth/2;
	if (itsTitleImage != NULL)
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
			f.SetColor(GetColormap()->GetInactiveLabelColor());
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
		p.SetPenColor(GetColormap()->GetInactiveLabelColor());
		p.RectInside(frame);
		}
}

/******************************************************************************
 PopUp

	Call this to make the menu pop open at the specified point inside
	mouseOwner.  This must only be called from mouseOwner's HandleMouseDown()
	or HandleMouseDrag().  If this function is successful, you will already
	have received HandleMouseUp() by the time it returns.  It will return kJTrue
	so you know that it succeeded and that you should return immediately.

	pt must be in the bounds coordinates of mouseOwner.  In addition, the
	menu must not be visible, because this would confuse the user.  We also
	require that the menu is not in a menu bar and is not a sub-menu.

	Try to avoid using this feature since it will usually not be obvious
	to the user that a menu can be popped up at all.

 ******************************************************************************/

JBoolean
JXMenu::PopUp
	(
	JXContainer*			mouseOwner,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	assert( itsIsHiddenPopupMenuFlag );

	if (itsMenuBar == NULL && itsOwner == NULL &&
		!IsVisible() && !buttonStates.AllOff() &&
		itsMenuDirector == NULL)
		{
		Place(pt.x, pt.y - GetFrameHeight());
		if (Open())
			{
			return GetDisplay()->SwitchDrag(mouseOwner, pt, buttonStates, modifiers,
											  itsMenuDirector->GetMenuTable());
			}
		}

	return kJFalse;
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
	if (button == kJXLeftButton && itsMenuDirector == NULL && Open())
		{
		const JBoolean ok =
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
	if (itsMenuDirector == NULL && Open())
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
	itsShouldBeActiveFlag = kJTrue;
	if ((itsBaseItemData->GetOrderedSet())->IsEmpty())
		{
		return;
		}

	const JBoolean wasActive = IsActive();
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
		itsShouldBeActiveFlag = kJFalse;
		}

	const JBoolean wasActive = IsActive();
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
	const JBoolean wasActive = IsActive();
	JXWidget::Suspend();
	if (itsOwner == NULL && wasActive && !IsActive())
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
	const JBoolean wasActive = IsActive();
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
	if (itsOwner != NULL &&
		!(itsBaseItemData->GetOrderedSet())->IsEmpty() &&
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
	if (itsOwner != NULL &&
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
	const JCollection* array = itsBaseItemData->GetOrderedSet();

	if (sender == const_cast<JCollection*>(array))
		{
		if (array->IsEmpty() && IsActive())
			{
			itsUpdateSBAFlag = kJFalse;
			Deactivate();
			itsUpdateSBAFlag = kJTrue;
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

JBoolean
JXMenu::IsMenu()
	const
{
	return kJTrue;
}
