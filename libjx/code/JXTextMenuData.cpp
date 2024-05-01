/******************************************************************************
 JXTextMenuData.cpp

	Stores a string and an image (optional) for each menu item.

	BASE CLASS = JXMenuData

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXTextMenuData.h"
#include "JXTextMenu.h"
#include "JXTextMenuTable.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXImage.h"
#include "jXMenuUtil.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JXTextMenuData::kImageChanged  = "ImageChanged::JXTextMenuData";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextMenuData::JXTextMenuData
	(
	JXTextMenu* menu
	)
	:
	JXMenuData(),
	itsMenu(menu),
	itsDefaultFont(JFontManager::GetDefaultFont())
{
	itsTextItemData = jnew JArray<TextItemData>;

	itsNeedGeomRecalcFlag = true;
	itsMaxImageWidth      = 1;
	itsMaxTextWidth       = 1;
	itsMaxShortcutWidth   = 1;
	itsHasNMShortcutsFlag = false;
	itsCompressHeightFlag = false;

	itsItemHeights = jnew JRunArray<JCoordinate>;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextMenuData::~JXTextMenuData()
{
	DeleteAll();
	jdelete itsTextItemData;

	jdelete itsItemHeights;
}

/******************************************************************************
 InsertItem

 ******************************************************************************/

void
JXTextMenuData::InsertItem
	(
	const JIndex			index,
	const JString&			str,
	const JXMenu::ItemType	type,
	const JString&			shortcuts,
	const JString&			nmShortcut,
	const JString&			id
	)
{
	auto* text = jnew JString(str);

	TextItemData itemData(text, itsDefaultFont);
	itsTextItemData->InsertItemAtIndex(index, itemData);

	JXMenuData::InsertItem(index, type, shortcuts, id);

	const JString* s;
	GetItemShortcuts(index, &s);
	itemData.ulIndex = JXWindow::GetULShortcutIndex(*itemData.text, s);
	itsTextItemData->SetItem(index, itemData);

	itsMenu->GetWindow()->MenuItemInserted(itsMenu, index);
	itsNeedGeomRecalcFlag = true;

	if (!nmShortcut.IsEmpty())
	{
		SetNMShortcut(index, nmShortcut);	// parse it and register it
	}
}

/******************************************************************************
 DeleteItem (virtual)

 ******************************************************************************/

void
JXTextMenuData::DeleteItem
	(
	const JIndex index
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	CleanOutTextItem(&itemData);
	itsTextItemData->RemoveItem(index);

	JXMenuData::DeleteItem(index);

	itsMenu->GetWindow()->MenuItemRemoved(itsMenu, index);
	itsNeedGeomRecalcFlag = true;
}

/******************************************************************************
 DeleteAll (virtual)

 ******************************************************************************/

void
JXTextMenuData::DeleteAll()
{
	for (auto itemData : *itsTextItemData)
	{
		CleanOutTextItem(&itemData);
	}
	itsTextItemData->RemoveAll();

	JXMenuData::DeleteAll();

	itsMenu->GetWindow()->ClearAllMenuShortcuts(itsMenu);
	itsNeedGeomRecalcFlag = true;
}

/******************************************************************************
 CleanOutTextItem (private)

 ******************************************************************************/

void
JXTextMenuData::CleanOutTextItem
	(
	TextItemData* itemData
	)
{
	jdelete itemData->text;
	itemData->text = nullptr;

	jdelete itemData->nmShortcut;
	itemData->nmShortcut = nullptr;

	if (itemData->ownsImage)
	{
		jdelete (itemData->image);
	}
	itemData->image = nullptr;
}

/******************************************************************************
 GetText

 ******************************************************************************/

const JString&
JXTextMenuData::GetText
	(
	const JIndex	index,
	JIndex*			ulIndex,
	JFont*			font
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetItem(index);
	*font    = itemData.font;
	*ulIndex = itemData.ulIndex;
	return *(itemData.text);
}

/******************************************************************************
 SetText

 ******************************************************************************/

void
JXTextMenuData::SetText
	(
	const JIndex	index,
	const JString&	str
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	*itemData.text = str;

	const JString* shortcuts;
	GetItemShortcuts(index, &shortcuts);
	itemData.ulIndex = JXWindow::GetULShortcutIndex(*itemData.text, shortcuts);
	itsTextItemData->SetItem(index, itemData);

	itsNeedGeomRecalcFlag = true;
}

/******************************************************************************
 SetMenuItems

	See InsertMenuItems() for documentation.

 ******************************************************************************/

void
JXTextMenuData::SetMenuItems
	(
	const JUtf8Byte* menuStr
	)
{
	DeleteAll();
	InsertMenuItems(1, menuStr);
}

/******************************************************************************
 InsertMenuItems

	%% is not supported.  The shortcuts can be listed after %h
	(e.g. "Quit %h Qq"), but they cannot include %.

	The vertical bar '|' separates menu items.

	See ParseMenuItemStr() for more details.

	If id is not nullptr, the string database is searched for "<item ID>"
	and this is parsed for the item's text and shortcuts.

 ******************************************************************************/

void
JXTextMenuData::InsertMenuItems
	(
	const JIndex		startIndex,
	const JUtf8Byte*	menuStr
	)
{
	JStringManager* strMgr = JGetStringManager();

	JPtrArray<JString> itemList(JPtrArrayT::kDeleteAll);
	JString(menuStr, JString::kNoCopy).Split("|", &itemList);

	const JSize itemCount = itemList.GetItemCount();
	JString itemText, shortcuts, nmShortcut, id, id1;
	for (JIndex i=1; i<=itemCount; i++)
	{
		itemText = *itemList.GetItem(i);

		bool isActive, hasSeparator;
		JXMenu::ItemType type;
		JXParseMenuItemStr(&itemText, &isActive, &hasSeparator,
						   &type, &shortcuts, &nmShortcut, &id);

		JString* itemText1;
		if (!id.IsEmpty() && strMgr->GetItem(id, &itemText1))
		{
			itemText = *itemText1;
			bool isActive1, hasSeparator1;
			JXMenu::ItemType type1;
			JXParseMenuItemStr(&itemText, &isActive1, &hasSeparator1,
							   &type1, &shortcuts, &nmShortcut, &id1);
		}

		if (JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle)
		{
			shortcuts.Clear();
		}

		const JIndex j = startIndex + i-1;
		InsertItem(j, itemText, type, shortcuts, nmShortcut, id);
		if (!isActive)
		{
			DisableItem(j);
		}
		if (hasSeparator)
		{
			ShowSeparatorAfter(j);
		}
	}
}

/******************************************************************************
 Set font

 ******************************************************************************/

void
JXTextMenuData::SetFontName
	(
	const JIndex	index,
	const JString&	name
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	itemData.font.SetName(name);
	itsTextItemData->SetItem(index, itemData);

	itsNeedGeomRecalcFlag = true;
}

void
JXTextMenuData::SetFontSize
	(
	const JIndex	index,
	const JSize		size
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	if (size != itemData.font.GetSize())
	{
		itemData.font.SetSize(size);
		itsTextItemData->SetItem(index, itemData);

		itsNeedGeomRecalcFlag = true;
	}
}

void
JXTextMenuData::SetFontStyle
	(
	const JIndex		index,
	const JFontStyle&	style
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	if (style != itemData.font.GetStyle())
	{
		itemData.font.SetStyle(style);
		itsTextItemData->SetItem(index, itemData);

		itsNeedGeomRecalcFlag = true;
	}
}

void
JXTextMenuData::SetFont
	(
	const JIndex	index,
	const JFont&	font
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	itemData.font = font;
	itsTextItemData->SetItem(index, itemData);

	itsNeedGeomRecalcFlag = true;
}

/******************************************************************************
 Set default font

 ******************************************************************************/

void
JXTextMenuData::SetDefaultFontName
	(
	const JString&	name,
	const bool	updateExisting
	)
{
	if (updateExisting)
	{
		JFont f = itsDefaultFont;
		f.SetName(name);
		UpdateItemFonts(itsDefaultFont, f);
	}

	itsDefaultFont.SetName(name);
}

void
JXTextMenuData::SetDefaultFontSize
	(
	const JSize		size,
	const bool	updateExisting
	)
{
	if (updateExisting)
	{
		JFont f = itsDefaultFont;
		f.SetSize(size);
		UpdateItemFonts(itsDefaultFont, f);
	}

	itsDefaultFont.SetSize(size);
}

void
JXTextMenuData::SetDefaultFontStyle
	(
	const JFontStyle&	style,
	const bool		updateExisting
	)
{
	if (updateExisting)
	{
		JFont f = itsDefaultFont;
		f.SetStyle(style);
		UpdateItemFonts(itsDefaultFont, f);
	}

	itsDefaultFont.SetStyle(style);
}

void
JXTextMenuData::SetDefaultFont
	(
	const JFont&	font,
	const bool	updateExisting
	)
{
	if (updateExisting)
	{
		UpdateItemFonts(itsDefaultFont, font);
	}

	itsDefaultFont = font;
}

/******************************************************************************
 UpdateItemFonts (private)

 ******************************************************************************/

void
JXTextMenuData::UpdateItemFonts
	(
	const JFont& oldFont,
	const JFont& newFont
	)
{
	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		TextItemData itemData = itsTextItemData->GetItem(i);
		if (itemData.font == oldFont)
		{
			itemData.font = newFont;
			itsTextItemData->SetItem(i, itemData);

			itsNeedGeomRecalcFlag = true;
		}
	}
}

/******************************************************************************
 GetImage

 ******************************************************************************/

bool
JXTextMenuData::GetImage
	(
	const JIndex	index,
	const JXImage**	image
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetItem(index);
	if (itemData.image != nullptr)
	{
		*image = itemData.image;
		return true;
	}
	else
	{
		*image = nullptr;
		return false;
	}
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXTextMenuData::SetImage
	(
	const JIndex	index,
	JXImage*		image,
	const bool		menuOwnsImage
	)
{
	if (image == nullptr)
	{
		ClearImage(index);
		return;
	}

	TextItemData itemData = itsTextItemData->GetItem(index);

	if (itemData.image == nullptr ||
		(itemData.image->GetBounds()) != image->GetBounds())
	{
		itsNeedGeomRecalcFlag = true;
	}

	if (itemData.ownsImage && image != itemData.image)
	{
		jdelete itemData.image;
	}

	itemData.image     = image;
	itemData.ownsImage = menuOwnsImage;
	itsTextItemData->SetItem(index, itemData);

	Broadcast(ImageChanged(index));
}

/******************************************************************************
 ClearImage

 ******************************************************************************/

void
JXTextMenuData::ClearImage
	(
	const JIndex index
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	if (itemData.image != nullptr)
	{
		if (itemData.ownsImage)
		{
			jdelete itemData.image;
		}
		itemData.image = nullptr;
		itsTextItemData->SetItem(index, itemData);

		itsNeedGeomRecalcFlag = true;
	}
}

/******************************************************************************
 GetNMShortcut

 ******************************************************************************/

bool
JXTextMenuData::GetNMShortcut
	(
	const JIndex	index,
	const JString**	str
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetItem(index);
	if (itemData.nmShortcut != nullptr)
	{
		*str = itemData.nmShortcut;
		return true;
	}
	else
	{
		*str = nullptr;
		return false;
	}
}

/******************************************************************************
 GetNMShortcut

 ******************************************************************************/

bool
JXTextMenuData::GetNMShortcut
	(
	const JIndex	index,
	const JString**	str,
	JFont*			font
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetItem(index);

	if (itemData.nmShortcut != nullptr)
	{
		*str  = itemData.nmShortcut;
		*font = itemData.font;
		return true;
	}
	else
	{
		*str = nullptr;
		*font = JFontManager::GetDefaultFont();
		return false;
	}
}

/******************************************************************************
 SetNMShortcut

 ******************************************************************************/

void
JXTextMenuData::SetNMShortcut
	(
	const JIndex	index,
	const JString&	str
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);

	const bool strEmpty = str.IsEmpty();

	bool changed = false;
	if (!strEmpty && itemData.nmShortcut == nullptr)
	{
		itemData.nmShortcut = jnew JString(str);
		itsTextItemData->SetItem(index, itemData);
		changed = true;
	}
	else if (!strEmpty)
	{
		*itemData.nmShortcut = str;
		changed = true;
	}
	else if (itemData.nmShortcut != nullptr)
	{
		jdelete itemData.nmShortcut;
		itemData.nmShortcut = nullptr;
		itsTextItemData->SetItem(index, itemData);
		changed = true;
	}

	if (changed)
	{
		itsNeedGeomRecalcFlag = true;

		JXWindow* window = itsMenu->GetWindow();
		window->ClearMenuShortcut(itsMenu, index);

		int key;
		JXKeyModifiers modifiers(itsMenu->GetDisplay());
		if (itemData.nmShortcut != nullptr &&
			JXParseNMShortcut(itemData.nmShortcut, &key, &modifiers, itsMenu->GetDisplay()->IsMacOS()) &&
			!window->InstallMenuShortcut(itsMenu, index, key, modifiers))
		{
			SetNMShortcut(index, JString::empty);	// don't display if not registered
		}
	}
}

/******************************************************************************
 ShowSeparatorAfter (protected)

 ******************************************************************************/

void
JXTextMenuData::ShowSeparatorAfter
	(
	const JIndex	index,
	const bool	show
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	if (itemData.separator != show)
	{
		itemData.separator = show;
		itsTextItemData->SetItem(index, itemData);

		itsNeedGeomRecalcFlag = true;
	}
}

/******************************************************************************
 ItemShortcutsChanged (virtual protected)

	Derived classes can override this to update underlining, etc.

	Note that shortcuts can be nullptr.

 ******************************************************************************/

void
JXTextMenuData::ItemShortcutsChanged
	(
	const JIndex	index,
	const JString*	shortcuts
	)
{
	TextItemData itemData = itsTextItemData->GetItem(index);
	itemData.ulIndex =
		JXWindow::GetULShortcutIndex(*(itemData.text), shortcuts);
	itsTextItemData->SetItem(index, itemData);
}

/******************************************************************************
 ConfigureTable

	Called by JXTextMenuTable constructor.

 ******************************************************************************/

void
JXTextMenuData::ConfigureTable
	(
	JXTextMenuTable* table
	)
{
	JFontManager* fontMgr = itsMenu->GetFontManager();

	const bool hasCheckboxes = HasCheckboxes();
	const bool hasSubmenus   = HasSubmenus();

	if (itsNeedGeomRecalcFlag)
	{
		itsNeedGeomRecalcFlag = false;
		itsMaxImageWidth      = 1;
		itsMaxTextWidth       = 1;
		itsMaxShortcutWidth   = JXMenuTable::kSubmenuColWidth;
		itsHasNMShortcutsFlag = false;
		itsItemHeights->RemoveAll();

		const JSize itemCount = itsTextItemData->GetItemCount();
		for (JIndex i=1; i<=itemCount; i++)
		{
			const TextItemData itemData = itsTextItemData->GetItem(i);
			JCoordinate h =
				(itsCompressHeightFlag && !hasCheckboxes && !hasSubmenus) ?
				0 : JXMenuTable::kMinRowHeight;

			if (itemData.text != nullptr && !itemData.text->IsEmpty())
			{
				JFont f = itemData.font;
				JXTextMenuTable::AdjustFont(itsMenu->GetDisplay(),
											JXTextMenuTable::kTextColumnIndex,
											*itemData.text, &f);

				h = JMax(h, (JCoordinate) f.GetLineHeight(fontMgr));

				itsMaxTextWidth = JMax(itsMaxTextWidth, (JCoordinate)
					JXTextMenuTable::GetTextWidth(fontMgr, f,
												  JXTextMenuTable::kTextColumnIndex,
												  *itemData.text));
			}

			if (itemData.image != nullptr)
			{
				h = JMax(h, itemData.image->GetHeight());
				itsMaxImageWidth = JMax(itsMaxImageWidth, itemData.image->GetWidth());
			}

			if (itemData.nmShortcut != nullptr)
			{
				itsHasNMShortcutsFlag = true;

				JFont f = itemData.font;
				JXTextMenuTable::AdjustFont(itsMenu->GetDisplay(),
											JXTextMenuTable::kSubmenuColumnIndex,
											*itemData.nmShortcut, &f);

				h = JMax(h, (JCoordinate) f.GetLineHeight(fontMgr));

				itsMaxShortcutWidth = JMax(itsMaxShortcutWidth, (JCoordinate)
					JXTextMenuTable::GetTextWidth(fontMgr, f,
												  JXTextMenuTable::kSubmenuColumnIndex,
												  *itemData.nmShortcut));
			}

			h += 2*(JXTextMenuTable::kHilightBorderWidth + 1);
			if (i < itemCount && itemData.separator)
			{
				h += JXTextMenuTable::kSeparatorHeight;
			}

			table->SetRowHeight(i,h);
			itsItemHeights->AppendItem(h);
		}
	}
	else
	{
		JRunArrayIterator<JCoordinate> iter(itsItemHeights);
		JCoordinate h;
		JIndex i=0;
		while (iter.Next(&h))
		{
			i++;
			table->SetRowHeight(i,h);
		}
	}

	// set the column widths

	if (hasCheckboxes)
	{
		table->SetColWidth(1, JCoordinate(JXMenuTable::kCheckboxColWidth) +
							  JCoordinate(JXTextMenuTable::kHilightBorderWidth));
	}
	else
	{
		table->SetColWidth(1, JXTextMenuTable::kHilightBorderWidth);
	}

	table->SetColWidth(2, itsMaxImageWidth);
	table->SetColWidth(3, itsMaxTextWidth);

	if (itsHasNMShortcutsFlag || hasSubmenus)
	{
		table->SetColWidth(4, itsMaxShortcutWidth +
							  JXTextMenuTable::kHilightBorderWidth);
	}
	else
	{
		table->SetColWidth(4, JXTextMenuTable::kHilightBorderWidth);
	}

	// set a sensible scroll step

	const JCoordinate scrollStep =
		itsDefaultFont.GetLineHeight(fontMgr) +
		2*(JXTextMenuTable::kHilightBorderWidth + 1);
	table->SetDefaultRowHeight(scrollStep);
}
