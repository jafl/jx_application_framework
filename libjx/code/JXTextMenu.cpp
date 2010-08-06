/******************************************************************************
 JXTextMenu.cpp

	A menu that displays an image (optional) and a styled string for each menu item.

	BASE CLASS = JXMenu

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTextMenu.h>
#include <JXTextMenuData.h>
#include <JXTextMenuDirector.h>
#include <JXDisplay.h>
#include <JXImageCache.h>
#include <JXImage.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextMenu::JXTextMenu
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
	JXMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXTextMenuX();
}

JXTextMenu::JXTextMenu
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
	itsTextMenuData = new JXTextMenuData(this);
	assert( itsTextMenuData != NULL );

	SetBaseItemData(itsTextMenuData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextMenu::~JXTextMenu()
{
	ClearBaseItemData();
	delete itsTextMenuData;
}

/******************************************************************************
 New item

 ******************************************************************************/

void
JXTextMenu::InsertItem
	(
	const JIndex		index,
	const JCharacter*	str,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	nmShortcut,
	const JCharacter*	id
	)
{
	itsTextMenuData->InsertItem(index, str, isCheckbox, isRadio, shortcuts, nmShortcut, id);
}

void
JXTextMenu::PrependItem
	(
	const JCharacter*	str,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	nmShortcut,
	const JCharacter*	id
	)
{
	itsTextMenuData->PrependItem(str, isCheckbox, isRadio, shortcuts, nmShortcut, id);
}

void
JXTextMenu::AppendItem
	(
	const JCharacter*	str,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	nmShortcut,
	const JCharacter*	id
	)
{
	itsTextMenuData->AppendItem(str, isCheckbox, isRadio, shortcuts, nmShortcut, id);
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
	const JIndex		index,
	const JCharacter*	str
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
	const JCharacter* menuStr,
	const JCharacter* idNamespace
	)
{
	itsTextMenuData->SetMenuItems(menuStr, idNamespace);
}

void
JXTextMenu::InsertMenuItems
	(
	const JIndex		index,
	const JCharacter*	menuStr,
	const JCharacter*	idNamespace
	)
{
	itsTextMenuData->InsertMenuItems(index, menuStr, idNamespace);
}

void
JXTextMenu::PrependMenuItems
	(
	const JCharacter* menuStr,
	const JCharacter* idNamespace
	)
{
	itsTextMenuData->PrependMenuItems(menuStr, idNamespace);
}

void
JXTextMenu::AppendMenuItems
	(
	const JCharacter* menuStr,
	const JCharacter* idNamespace
	)
{
	itsTextMenuData->AppendMenuItems(menuStr, idNamespace);
}

/******************************************************************************
 Get font

 ******************************************************************************/

const JCharacter*
JXTextMenu::GetItemFontName
	(
	const JIndex index
	)
	const
{
	return itsTextMenuData->GetFontName(index);
}

JSize
JXTextMenu::GetItemFontSize
	(
	const JIndex index
	)
	const
{
	return itsTextMenuData->GetFontSize(index);
}

JFontStyle
JXTextMenu::GetItemFontStyle
	(
	const JIndex index
	)
	const
{
	return itsTextMenuData->GetFontStyle(index);
}

void
JXTextMenu::GetItemFont
	(
	const JIndex	index,
	JString*		name,
	JSize*			size,
	JFontStyle*		style
	)
	const
{
	itsTextMenuData->GetFont(index, name, size, style);
}

void
JXTextMenu::GetItemFont
	(
	const JIndex	index,
	JFontID*		id,
	JSize*			size,
	JFontStyle*		style
	)
	const
{
	itsTextMenuData->GetFont(index, id, size, style);
}

/******************************************************************************
 Set font

 ******************************************************************************/

void
JXTextMenu::SetItemFontName
	(
	const JIndex		index,
	const JCharacter*	name
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
	const JIndex		index,
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
{
	itsTextMenuData->SetFont(index, name, size, style);
}

void
JXTextMenu::SetItemFont
	(
	const JIndex		index,
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style
	)
{
	itsTextMenuData->SetFont(index, id, size, style);
}

/******************************************************************************
 Get default font

 ******************************************************************************/

const JCharacter*
JXTextMenu::GetDefaultFontName()
	const
{
	return itsTextMenuData->GetDefaultFontName();
}

JSize
JXTextMenu::GetDefaultFontSize()
	const
{
	return itsTextMenuData->GetDefaultFontSize();
}

const JFontStyle&
JXTextMenu::GetDefaultFontStyle()
	const
{
	return itsTextMenuData->GetDefaultFontStyle();
}

void
JXTextMenu::GetDefaultFont
	(
	JString*	name,
	JSize*		size,
	JFontStyle*	style
	)
	const
{
	itsTextMenuData->GetDefaultFont(name, size, style);
}

void
JXTextMenu::GetDefaultFont
	(
	JFontID*	id,
	JSize*		size,
	JFontStyle*	style
	)
	const
{
	itsTextMenuData->GetDefaultFont(id, size, style);
}

/******************************************************************************
 Set default font

 ******************************************************************************/

void
JXTextMenu::SetDefaultFontName
	(
	const JCharacter*	name,
	const JBoolean		updateExisting
	)
{
	itsTextMenuData->SetDefaultFontName(name, updateExisting);
}

void
JXTextMenu::SetDefaultFontSize
	(
	const JSize		size,
	const JBoolean	updateExisting
	)
{
	itsTextMenuData->SetDefaultFontSize(size, updateExisting);
}

void
JXTextMenu::SetDefaultFontStyle
	(
	const JFontStyle&	style,
	const JBoolean		updateExisting
	)
{
	itsTextMenuData->SetDefaultFontStyle(style, updateExisting);
}

void
JXTextMenu::SetDefaultFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const JBoolean		updateExisting
	)
{
	itsTextMenuData->SetDefaultFont(name, size, style, updateExisting);
}

void
JXTextMenu::SetDefaultFont
	(
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style,
	const JBoolean		updateExisting
	)
{
	itsTextMenuData->SetDefaultFont(id, size, style, updateExisting);
}

/******************************************************************************
 Item image

 ******************************************************************************/

JBoolean
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
	const JBoolean	menuOwnsImage
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
	if (image->GetXColormap() == GetColormap())
		{
		SetItemImage(index, image, kJFalse);
		}
	else
		{
		JXImage* image = new JXImage(GetDisplay(), data);
		assert( image != NULL );
		SetItemImage(index, image, kJTrue);
		}
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

JBoolean
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
		return kJTrue;
		}
	else
		{
		str->Clear();
		return kJFalse;
		}
}

void
JXTextMenu::SetItemNMShortcut
	(
	const JIndex		index,
	const JCharacter*	str
	)
{
	itsTextMenuData->SetNMShortcut(index, str);
}

/******************************************************************************
 Separator

 ******************************************************************************/

JBoolean
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
	const JBoolean	show
	)
{
	itsTextMenuData->ShowSeparatorAfter(index, show);
}

/******************************************************************************
 Compress height

 ******************************************************************************/

JBoolean
JXTextMenu::HeightCompressed()
	const
{
	return itsTextMenuData->HeightCompressed();
}

void
JXTextMenu::CompressHeight
	(
	const JBoolean compress
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

	if (PrepareToOpenMenu() &&
		itsTextMenuData->IndexValid(index) && itsTextMenuData->IsEnabled(index))
		{
		BroadcastSelection(index, kJTrue);
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
	JXTextMenuDirector* dir =
		new JXTextMenuDirector(supervisor, this, itsTextMenuData);
	assert( dir != NULL );
	return dir;
}

/******************************************************************************
 SetToPopupChoice (virtual)

 ******************************************************************************/

void
JXTextMenu::SetToPopupChoice
	(
	const JBoolean	isPopup,
	const JIndex	initialChoice
	)
{
	const JString& origTitle = GetTitleText();
	if (isPopup && !origTitle.IsEmpty() && !origTitle.EndsWith(":"))
		{
		JString newTitle = origTitle;
		newTitle.AppendCharacter(':');
		SetTitle(newTitle, NULL, kJFalse);
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
	if (!origTitle.IsEmpty())
		{
		JString newTitle;
		JIndex colonIndex;
		const JBoolean foundColon = origTitle.LocateSubstring(":", &colonIndex);
		if (foundColon && colonIndex > 1)
			{
			newTitle = origTitle.GetSubstring(1, colonIndex-1);
			}
		// not empty but no colon => title was empty originally, so replace it

		if (!newTitle.IsEmpty())
			{
			newTitle += ":  ";
			}
		newTitle += GetItemText(index);

		const JXImage* image = NULL;
		GetItemImage(index, &image);

		SetTitle(newTitle, const_cast<JXImage*>(image), kJFalse);
		}
}
