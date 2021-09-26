/******************************************************************************
 JXTextMenu.cpp

	A menu that displays an image (optional) and a styled string for each menu item.

	BASE CLASS = JXMenu

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXTextMenu.h"
#include "jx-af/jx/JXTextMenuData.h"
#include "jx-af/jx/JXTextMenuDirector.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXImageCache.h"
#include "jx-af/jx/JXImage.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextMenu::JXTextMenu
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
	JXMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXTextMenuX();
}

JXTextMenu::JXTextMenu
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
	JXMenu(image, menuOwnsImage, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXTextMenuX();
}

JXTextMenu::JXTextMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXMenu(owner, itemIndex, enclosure)
{
	JXTextMenuX();
}

// private

void
JXTextMenu::JXTextMenuX()
{
	itsTextMenuData = jnew JXTextMenuData(this);
	assert( itsTextMenuData != nullptr );

	SetBaseItemData(itsTextMenuData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextMenu::~JXTextMenu()
{
	ClearBaseItemData();
	jdelete itsTextMenuData;
}

/******************************************************************************
 New item

 ******************************************************************************/

void
JXTextMenu::InsertItem
	(
	const JIndex	index,
	const JString&	str,
	const ItemType	type,
	const JString&	shortcuts,
	const JString&	nmShortcut,
	const JString&	id
	)
{
	itsTextMenuData->InsertItem(index, str, type, shortcuts, nmShortcut, id);
}

void
JXTextMenu::PrependItem
	(
	const JString&	str,
	const ItemType	type,
	const JString&	shortcuts,
	const JString&	nmShortcut,
	const JString&	id
	)
{
	itsTextMenuData->PrependItem(str, type, shortcuts, nmShortcut, id);
}

void
JXTextMenu::AppendItem
	(
	const JString&	str,
	const ItemType	type,
	const JString&	shortcuts,
	const JString&	nmShortcut,
	const JString&	id
	)
{
	itsTextMenuData->AppendItem(str, type, shortcuts, nmShortcut, id);
}

/******************************************************************************
 Item text

 ******************************************************************************/

const JString&
JXTextMenu::GetItemText
	(
	const JIndex index
	)
	const
{
	return itsTextMenuData->GetText(index);
}

void
JXTextMenu::SetItemText
	(
	const JIndex	index,
	const JString&	str
	)
{
	itsTextMenuData->SetText(index, str);
}

/******************************************************************************
 Add multiple items

 ******************************************************************************/

void
JXTextMenu::SetMenuItems
	(
	const JUtf8Byte* menuStr,
	const JUtf8Byte* idNamespace
	)
{
	itsTextMenuData->SetMenuItems(menuStr, idNamespace);
}

void
JXTextMenu::InsertMenuItems
	(
	const JIndex		index,
	const JUtf8Byte*	menuStr,
	const JUtf8Byte*	idNamespace
	)
{
	itsTextMenuData->InsertMenuItems(index, menuStr, idNamespace);
}

void
JXTextMenu::PrependMenuItems
	(
	const JUtf8Byte* menuStr,
	const JUtf8Byte* idNamespace
	)
{
	itsTextMenuData->PrependMenuItems(menuStr, idNamespace);
}

void
JXTextMenu::AppendMenuItems
	(
	const JUtf8Byte* menuStr,
	const JUtf8Byte* idNamespace
	)
{
	itsTextMenuData->AppendMenuItems(menuStr, idNamespace);
}

/******************************************************************************
 Get font

 ******************************************************************************/

JFont
JXTextMenu::GetItemFont
	(
	const JIndex index
	)
	const
{
	return itsTextMenuData->GetFont(index);
}

/******************************************************************************
 Set font

 ******************************************************************************/

void
JXTextMenu::SetItemFontName
	(
	const JIndex	index,
	const JString&	name
	)
{
	itsTextMenuData->SetFontName(index, name);
}

void
JXTextMenu::SetItemFontSize
	(
	const JIndex	index,
	const JSize		size
	)
{
	itsTextMenuData->SetFontSize(index, size);
}

void
JXTextMenu::SetItemFontStyle
	(
	const JIndex		index,
	const JFontStyle&	style
	)
{
	itsTextMenuData->SetFontStyle(index, style);
}

void
JXTextMenu::SetItemFont
	(
	const JIndex	index,
	const JFont&	font
	)
{
	itsTextMenuData->SetFont(index, font);
}

/******************************************************************************
 Get default font

 ******************************************************************************/

const JFont&
JXTextMenu::GetDefaultFont()
	const
{
	return itsTextMenuData->GetDefaultFont();
}

/******************************************************************************
 Set default font

 ******************************************************************************/

void
JXTextMenu::SetDefaultFontName
	(
	const JString&	name,
	const bool	updateExisting
	)
{
	itsTextMenuData->SetDefaultFontName(name, updateExisting);
}

void
JXTextMenu::SetDefaultFontSize
	(
	const JSize		size,
	const bool	updateExisting
	)
{
	itsTextMenuData->SetDefaultFontSize(size, updateExisting);
}

void
JXTextMenu::SetDefaultFontStyle
	(
	const JFontStyle&	style,
	const bool		updateExisting
	)
{
	itsTextMenuData->SetDefaultFontStyle(style, updateExisting);
}

void
JXTextMenu::SetDefaultFont
	(
	const JFont&	font,
	const bool	updateExisting
	)
{
	itsTextMenuData->SetDefaultFont(font, updateExisting);
}

/******************************************************************************
 Item image

 ******************************************************************************/

bool
JXTextMenu::GetItemImage
	(
	const JIndex	index,
	const JXImage**	image
	)
	const
{
	return itsTextMenuData->GetImage(index, image);
}

void
JXTextMenu::SetItemImage
	(
	const JIndex	index,
	JXImage*		image,
	const bool	menuOwnsImage
	)
{
	itsTextMenuData->SetImage(index, image, menuOwnsImage);
}

void
JXTextMenu::SetItemImage
	(
	const JIndex	index,
	const JXPM&		data
	)
{
	JXImage* image = GetDisplay()->GetImageCache()->GetImage(data);
	SetItemImage(index, image, false);
}

void
JXTextMenu::ClearItemImage
	(
	const JIndex index
	)
{
	itsTextMenuData->ClearImage(index);
}

/******************************************************************************
 Item non-menu shortcut

 ******************************************************************************/

bool
JXTextMenu::GetItemNMShortcut
	(
	const JIndex	index,
	JString*		str
	)
	const
{
	const JString* s;
	if (itsTextMenuData->GetNMShortcut(index, &s))
	{
		*str = *s;
		return true;
	}
	else
	{
		str->Clear();
		return false;
	}
}

void
JXTextMenu::SetItemNMShortcut
	(
	const JIndex	index,
	const JString&	str
	)
{
	itsTextMenuData->SetNMShortcut(index, str);
}

/******************************************************************************
 Separator

 ******************************************************************************/

bool
JXTextMenu::HasSeparatorAfter
	(
	const JIndex index
	)
	const
{
	return itsTextMenuData->HasSeparator(index);
}

void
JXTextMenu::ShowSeparatorAfter
	(
	const JIndex	index,
	const bool	show
	)
{
	itsTextMenuData->ShowSeparatorAfter(index, show);
}

/******************************************************************************
 Compress height

 ******************************************************************************/

bool
JXTextMenu::HeightCompressed()
	const
{
	return itsTextMenuData->HeightCompressed();
}

void
JXTextMenu::CompressHeight
	(
	const bool compress
	)
{
	itsTextMenuData->CompressHeight(compress);
}

/******************************************************************************
 HandleNMShortcut (virtual)

 ******************************************************************************/

void
JXTextMenu::HandleNMShortcut
	(
	const JIndex			index,
	const JXKeyModifiers&	modifiers
	)
{
	// Update menu items so active setting is correct
	// and then broadcast the selection.

	if (PrepareToOpenMenu(true) &&
		itsTextMenuData->IndexValid(index) && itsTextMenuData->IsEnabled(index))
	{
		BroadcastSelection(index, true);
	}
}

/******************************************************************************
 CreateMenuWindow (virtual protected)

 ******************************************************************************/

JXMenuDirector*
JXTextMenu::CreateMenuWindow
	(
	JXWindowDirector* supervisor
	)
{
	auto* dir =
		jnew JXTextMenuDirector(supervisor, this, itsTextMenuData);
	assert( dir != nullptr );
	return dir;
}

/******************************************************************************
 SetToPopupChoice (virtual)

 ******************************************************************************/

void
JXTextMenu::SetToPopupChoice
	(
	const bool	isPopup,
	const JIndex	initialChoice
	)
{
	const JString& origTitle = GetTitleText();
	if (isPopup && !origTitle.IsEmpty() && !origTitle.EndsWith(":"))
	{
		JString newTitle = origTitle;
		newTitle.Append(":");
		SetTitle(newTitle, nullptr, false);
	}

	JXMenu::SetToPopupChoice(isPopup, initialChoice);
}

/******************************************************************************
 AdjustPopupChoiceTitle (virtual protected)

 ******************************************************************************/

void
JXTextMenu::AdjustPopupChoiceTitle
	(
	const JIndex index
	)
{
	const JString& origTitle = GetTitleText();
	if (origTitle.IsEmpty())
	{
		return;
	}

	JString newTitle = origTitle;

	JStringIterator iter(&newTitle);
	if (iter.Next(":"))
	{
		iter.SkipPrev();
		iter.RemoveAllNext();
	}
	iter.Invalidate();

	if (!newTitle.IsEmpty())
	{
		newTitle += ":  ";
	}
	newTitle += GetItemText(index);

	const JXImage* image = nullptr;
	GetItemImage(index, &image);

	SetTitle(newTitle, const_cast<JXImage*>(image), false);
}
