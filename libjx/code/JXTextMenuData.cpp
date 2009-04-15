/******************************************************************************
 JXTextMenuData.cpp

	Stores a string and an image (optional) for each menu item.

	BASE CLASS = JXMenuData

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTextMenuData.h>
#include <JXTextMenu.h>
#include <JXTextMenuTable.h>
#include <JXWindow.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXKeysym.h>
#include <JFontManager.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JXTextMenuData::kImageChanged  = "ImageChanged::JXTextMenuData";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextMenuData::JXTextMenuData
	(
	JXTextMenu* menu
	)
	:
	JXMenuData(),
	itsMenu( menu ),
	itsFontMgr( menu->GetFontManager() )
{
	itsTextItemData = new JArray<TextItemData>;
	assert( itsTextItemData != NULL );

	const JCharacter* defFontName = JXMenu::GetDefaultFont(&itsDefFontSize);
	// itsDefFontStyle automatically initialized to empty
	itsDefFontID = itsFontMgr->GetFontID(defFontName, itsDefFontSize,
										 itsDefFontStyle);

	itsNeedGeomRecalcFlag = kJTrue;
	itsMaxImageWidth      = 1;
	itsMaxTextWidth       = 1;
	itsMaxShortcutWidth   = 1;
	itsHasNMShortcutsFlag = kJFalse;
	itsCompressHeightFlag = kJFalse;

	itsItemHeights = new JRunArray<JCoordinate>;
	assert( itsItemHeights != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextMenuData::~JXTextMenuData()
{
	DeleteAll();
	delete itsTextItemData;

	delete itsItemHeights;
}

/******************************************************************************
 InsertItem

 ******************************************************************************/

void
JXTextMenuData::InsertItem
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
	JString* text = new JString(str);
	assert( text != NULL );

	TextItemData itemData(text, itsDefFontID, itsDefFontSize, itsDefFontStyle);
	itsTextItemData->InsertElementAtIndex(index, itemData);

	JXMenuData::InsertItem(index, isCheckbox, isRadio, shortcuts, id);

	const JString* s;
	GetItemShortcuts(index, &s);
	itemData.ulIndex = JXWindow::GetULShortcutIndex(*(itemData.text), s);
	itsTextItemData->SetElement(index, itemData);

	(itsMenu->GetWindow())->MenuItemInserted(itsMenu, index);
	itsNeedGeomRecalcFlag = kJTrue;

	SetNMShortcut(index, nmShortcut);	// parse it and register it
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
	TextItemData itemData = itsTextItemData->GetElement(index);
	CleanOutTextItem(&itemData);
	itsTextItemData->RemoveElement(index);

	JXMenuData::DeleteItem(index);

	(itsMenu->GetWindow())->MenuItemRemoved(itsMenu, index);
	itsNeedGeomRecalcFlag = kJTrue;
}

/******************************************************************************
 DeleteAll (virtual)

 ******************************************************************************/

void
JXTextMenuData::DeleteAll()
{
	const JSize itemCount = itsTextItemData->GetElementCount();
	for (JIndex i=1; i<=itemCount; i++)
		{
		TextItemData itemData = itsTextItemData->GetElement(i);
		CleanOutTextItem(&itemData);
		}
	itsTextItemData->RemoveAll();

	JXMenuData::DeleteAll();

	(itsMenu->GetWindow())->ClearAllMenuShortcuts(itsMenu);
	itsNeedGeomRecalcFlag = kJTrue;
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
	delete (itemData->text);
	itemData->text = NULL;

	delete (itemData->nmShortcut);
	itemData->nmShortcut = NULL;

	if (itemData->ownsImage)
		{
		delete (itemData->image);
		}
	itemData->image = NULL;
}

/******************************************************************************
 GetText

 ******************************************************************************/

const JString&
JXTextMenuData::GetText
	(
	const JIndex	index,
	JIndex*			ulIndex,
	JFontID*		id,
	JSize*			size,
	JFontStyle*		style
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	*id    = itemData.fontID;
	*size  = itemData.fontSize;
	*style = itemData.fontStyle;

	*ulIndex = itemData.ulIndex;
	return *(itemData.text);
}

/******************************************************************************
 SetText

 ******************************************************************************/

void
JXTextMenuData::SetText
	(
	const JIndex		index,
	const JCharacter*	str
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	*(itemData.text) = str;

	const JString* shortcuts;
	GetItemShortcuts(index, &shortcuts);
	itemData.ulIndex = JXWindow::GetULShortcutIndex(*(itemData.text), shortcuts);
	itsTextItemData->SetElement(index, itemData);

	itsNeedGeomRecalcFlag = kJTrue;
}

/******************************************************************************
 SetMenuItems

	See InsertMenuItems() for documentation.

 ******************************************************************************/

void
JXTextMenuData::SetMenuItems
	(
	const JCharacter* menuStr,
	const JCharacter* idNamespace
	)
{
	DeleteAll();
	InsertMenuItems(1, menuStr, idNamespace);
}

/******************************************************************************
 InsertMenuItems

	%% is not supported.  The shortcuts can be listed after %h
	(e.g. "Quit %h Qq"), but they cannot include %.

	The vertical bar '|' separates menu items.

	See ParseMenuItemStr() for more details.

	If idNamespace is not NULL, the string database is searched for
	"<item ID>::<idNamespace>" and this is parsed for the item's text
	and shortcuts.

 ******************************************************************************/

void
JXTextMenuData::InsertMenuItems
	(
	const JIndex		startIndex,
	const JCharacter*	menuStr,
	const JCharacter*	idNamespace
	)
{
	JStringManager* strMgr = JGetStringManager();

	JSize currIndex = startIndex;
	JString str     = menuStr;
	JBoolean done   = kJFalse;
	JString itemText, shortcuts, nmShortcut, id, strID, id1;
	while (!done)
		{
		JIndex sepIndex;
		const JBoolean found = str.LocateSubstring("|", &sepIndex);
		if (found)
			{
			assert( sepIndex > 1 );
			itemText = str.GetSubstring(1, sepIndex-1);
			str.RemoveSubstring(1, sepIndex);
			}
		else
			{
			itemText = str;
			done     = kJTrue;
			}

		JBoolean isActive, hasSeparator, isCheckbox, isRadio;
		ParseMenuItemStr(&itemText, &isActive, &hasSeparator,
						 &isCheckbox, &isRadio, &shortcuts, &nmShortcut, &id);

		if (!JStringEmpty(idNamespace) && !id.IsEmpty())
			{
			strID  = id;
			strID += "::";
			strID += idNamespace;
			JString* itemText1;
			if (strMgr->GetElement(strID, &itemText1) && itemText1 != NULL)
				{
				itemText = *itemText1;
				JBoolean isActive1, hasSeparator1, isCheckbox1, isRadio1;
				ParseMenuItemStr(&itemText, &isActive1, &hasSeparator1,
								 &isCheckbox1, &isRadio1, &shortcuts, &nmShortcut, &id1);
				}
			}

		InsertItem(currIndex, itemText, isCheckbox, isRadio, shortcuts, nmShortcut, id);
		if (!isActive)
			{
			DisableItem(currIndex);
			}
		if (hasSeparator)
			{
			ShowSeparatorAfter(currIndex);
			}
		currIndex++;
		}
}

/******************************************************************************
 ParseMenuItemStr (private)

	%d = disabled, %l = separator line after item,
	%b = checkbox, %r = radio button,
	%h<chars> = specify shortcuts, %k<chars> = specify non-menu shortcut string,
	%i<chars> = internal identifier

 ******************************************************************************/

void
JXTextMenuData::ParseMenuItemStr
	(
	JString*	text,
	JBoolean*	isActive,
	JBoolean*	hasSeparator,
	JBoolean*	isCheckbox,
	JBoolean*	isRadio,
	JString*	shortcuts,
	JString*	nmShortcut,
	JString*	id
	)
	const
{
	*isActive     = kJTrue;
	*hasSeparator = kJFalse;
	*isCheckbox   = kJFalse;
	*isRadio      = kJFalse;

	shortcuts->Clear();
	nmShortcut->Clear();
	id->Clear();

	JIndex opIndex;
	while (text->LocateLastSubstring("%", &opIndex))
		{
		const JSize textLength = text->GetLength();
		if (opIndex == textLength)	// can't assert() because of menu_strings file
			{
			text->RemoveSubstring(textLength, textLength);
			continue;
			}

		JString op = text->GetSubstring(opIndex+1, textLength);
		text->RemoveSubstring(opIndex, textLength);

		const JCharacter opc = op.GetFirstCharacter();
		if (opc == 'd')
			{
			*isActive = kJFalse;
			}
		else if (opc == 'l')
			{
			*hasSeparator = kJTrue;
			}
		else if (opc == 'b' || opc == 'B')
			{
			*isCheckbox = kJTrue;
			}
		else if (opc == 'r' || opc == 'R')
			{
			*isCheckbox = kJTrue;
			*isRadio    = kJTrue;
			}

		else if (opc == 'h' && shortcuts->IsEmpty())
			{
			*shortcuts = op.GetSubstring(2, op.GetLength());
			shortcuts->TrimWhitespace();
			}
		else if (opc == 'h')
			{
			cerr << "Tried to use %h more than once in '" << *text << '\'' << endl;
			}

		else if (opc == 'k' && nmShortcut->IsEmpty())
			{
			*nmShortcut = op.GetSubstring(2, op.GetLength());
			nmShortcut->TrimWhitespace();
			}
		else if (opc == 'k')
			{
			cerr << "Tried to use %k more than once in '" << *text << '\'' << endl;
			}

		else if (opc == 'i' && id->IsEmpty())
			{
			*id = op.GetSubstring(2, op.GetLength());
			id->TrimWhitespace();
			}
		else if (opc == 'i')
			{
			cerr << "Tried to use %i more than once in '" << *text << '\'' << endl;
			}

		else
			{
			cerr << "Unsupported option %" << op << " in '" << *text << '\'' << endl;
			}
		}

	text->TrimWhitespace();
}

/******************************************************************************
 Get font

 ******************************************************************************/

const JCharacter*
JXTextMenuData::GetFontName
	(
	const JIndex index
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	return itsFontMgr->GetFontName(itemData.fontID);
}

void
JXTextMenuData::GetFont
	(
	const JIndex	index,
	JString*		name,
	JSize*			size,
	JFontStyle*		style
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	*name  = itsFontMgr->GetFontName(itemData.fontID);
	*size  = itemData.fontSize;
	*style = itemData.fontStyle;
}

/******************************************************************************
 Set font

 ******************************************************************************/

void
JXTextMenuData::SetFontName
	(
	const JIndex		index,
	const JCharacter*	name
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	const JFontID newID   = itsFontMgr->GetFontID(name, itemData.fontSize,
												  itemData.fontStyle);
	if (newID != itemData.fontID)
		{
		itemData.fontID = newID;
		itsTextItemData->SetElement(index, itemData);

		itsNeedGeomRecalcFlag = kJTrue;
		}
}

void
JXTextMenuData::SetFontSize
	(
	const JIndex	index,
	const JSize		size
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	if (size != itemData.fontSize)
		{
		itemData.fontSize = size;

		itemData.fontID = itsFontMgr->UpdateFontID(itemData.fontID, itemData.fontSize,
												   itemData.fontStyle);
		itsTextItemData->SetElement(index, itemData);

		itsNeedGeomRecalcFlag = kJTrue;
		}
}

void
JXTextMenuData::SetFontStyle
	(
	const JIndex		index,
	const JFontStyle&	style
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	if (style != itemData.fontStyle)
		{
		itemData.fontStyle = style;

		itemData.fontID = itsFontMgr->UpdateFontID(itemData.fontID, itemData.fontSize,
												   itemData.fontStyle);
		itsTextItemData->SetElement(index, itemData);

		itsNeedGeomRecalcFlag = kJTrue;
		}
}

void
JXTextMenuData::SetFont
	(
	const JIndex		index,
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	itemData.fontID    = itsFontMgr->GetFontID(name, size, style);
	itemData.fontSize  = size;
	itemData.fontStyle = style;
	itsTextItemData->SetElement(index, itemData);

	itsNeedGeomRecalcFlag = kJTrue;
}

void
JXTextMenuData::SetFont
	(
	const JIndex		index,
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	itemData.fontID    = id;
	itemData.fontSize  = size;
	itemData.fontStyle = style;
	itsTextItemData->SetElement(index, itemData);

	itsNeedGeomRecalcFlag = kJTrue;
}

/******************************************************************************
 Get default font

 ******************************************************************************/

const JCharacter*
JXTextMenuData::GetDefaultFontName()
	const
{
	return itsFontMgr->GetFontName(itsDefFontID);
}

void
JXTextMenuData::GetDefaultFont
	(
	JString*	name,
	JSize*		size,
	JFontStyle*	style
	)
	const
{
	*name  = itsFontMgr->GetFontName(itsDefFontID);
	*size  = itsDefFontSize;
	*style = itsDefFontStyle;
}

/******************************************************************************
 Set default font

 ******************************************************************************/

void
JXTextMenuData::SetDefaultFontName
	(
	const JCharacter*	name,
	const JBoolean		updateExisting
	)
{
	const JFontID id = itsFontMgr->GetFontID(name, itsDefFontSize,
											 itsDefFontStyle);
	if (updateExisting)
		{
		UpdateItemFonts(itsDefFontID, itsDefFontSize, itsDefFontStyle,
						id,           itsDefFontSize, itsDefFontStyle);
		}

	itsDefFontID = id;
}

void
JXTextMenuData::SetDefaultFontSize
	(
	const JSize		size,
	const JBoolean	updateExisting
	)
{
	const JFontID id = itsFontMgr->UpdateFontID(itsDefFontID, size,
												itsDefFontStyle);
	if (updateExisting)
		{
		UpdateItemFonts(itsDefFontID, itsDefFontSize, itsDefFontStyle,
						id,           size,           itsDefFontStyle);
		}

	itsDefFontID   = id;
	itsDefFontSize = size;
}

void
JXTextMenuData::SetDefaultFontStyle
	(
	const JFontStyle&	style,
	const JBoolean		updateExisting
	)
{
	const JFontID id = itsFontMgr->UpdateFontID(itsDefFontID, itsDefFontSize,
												style);
	if (updateExisting)
		{
		UpdateItemFonts(itsDefFontID, itsDefFontSize, itsDefFontStyle,
						id,           itsDefFontSize, style);
		}

	itsDefFontID   = id;
	itsDefFontStyle = style;
}

void
JXTextMenuData::SetDefaultFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const JBoolean		updateExisting
	)
{
	const JFontID id = itsFontMgr->GetFontID(name, size, style);
	if (updateExisting)
		{
		UpdateItemFonts(itsDefFontID, itsDefFontSize, itsDefFontStyle,
						id,           size,           style);
		}

	itsDefFontID    = id;
	itsDefFontSize  = size;
	itsDefFontStyle = style;
}

void
JXTextMenuData::SetDefaultFont
	(
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style,
	const JBoolean		updateExisting
	)
{
	if (updateExisting)
		{
		UpdateItemFonts(itsDefFontID, itsDefFontSize, itsDefFontStyle,
						id,           size,           style);
		}

	itsDefFontID    = id;
	itsDefFontSize  = size;
	itsDefFontStyle = style;
}

/******************************************************************************
 UpdateItemFonts (private)

 ******************************************************************************/

void
JXTextMenuData::UpdateItemFonts
	(
	const JFontID		oldID,
	const JSize			oldSize,
	const JFontStyle&	oldStyle,

	const JFontID		newID,
	const JSize			newSize,
	const JFontStyle&	newStyle
	)
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		TextItemData itemData = itsTextItemData->GetElement(i);
		if (itemData.fontID    == oldID &&
			itemData.fontSize  == oldSize &&
			itemData.fontStyle == oldStyle)
			{
			itemData.fontID    = newID;
			itemData.fontSize  = newSize;
			itemData.fontStyle = newStyle;
			itsTextItemData->SetElement(i, itemData);

			itsNeedGeomRecalcFlag = kJTrue;
			}
		}
}

/******************************************************************************
 GetImage

 ******************************************************************************/

JBoolean
JXTextMenuData::GetImage
	(
	const JIndex	index,
	const JXImage**	image
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	if (itemData.image != NULL)
		{
		*image = itemData.image;
		return kJTrue;
		}
	else
		{
		*image = NULL;
		return kJFalse;
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
	const JBoolean	menuOwnsImage
	)
{
	if (image == NULL)
		{
		ClearImage(index);
		return;
		}

	TextItemData itemData = itsTextItemData->GetElement(index);

	if (itemData.image == NULL ||
		(itemData.image->GetBounds()) != image->GetBounds())
		{
		itsNeedGeomRecalcFlag = kJTrue;
		}

	if (itemData.ownsImage && image != itemData.image)
		{
		delete itemData.image;
		}

	itemData.image     = image;
	itemData.ownsImage = menuOwnsImage;
	itsTextItemData->SetElement(index, itemData);

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
	TextItemData itemData = itsTextItemData->GetElement(index);
	if (itemData.image != NULL)
		{
		if (itemData.ownsImage)
			{
			delete itemData.image;
			}
		itemData.image = NULL;
		itsTextItemData->SetElement(index, itemData);

		itsNeedGeomRecalcFlag = kJTrue;
		}
}

/******************************************************************************
 GetNMShortcut

 ******************************************************************************/

JBoolean
JXTextMenuData::GetNMShortcut
	(
	const JIndex	index,
	const JString**	str
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	if (itemData.nmShortcut != NULL)
		{
		*str = itemData.nmShortcut;
		return kJTrue;
		}
	else
		{
		*str = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 GetNMShortcut

 ******************************************************************************/

JBoolean
JXTextMenuData::GetNMShortcut
	(
	const JIndex	index,
	const JString**	str,
	JFontID*		id,
	JSize*			size,
	JFontStyle*		style
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);

	if (itemData.nmShortcut != NULL)
		{
		*str = itemData.nmShortcut;

		*size  = itemData.fontSize;
		*style = JFontStyle();
		*id    = itsFontMgr->UpdateFontID(itemData.fontID, *size, *style);

		return kJTrue;
		}
	else
		{
		*str = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 SetNMShortcut

 ******************************************************************************/

void
JXTextMenuData::SetNMShortcut
	(
	const JIndex		index,
	const JCharacter*	str
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);

	const JBoolean strEmpty = JStringEmpty(str);

	JBoolean changed = kJFalse;
	if (!strEmpty && itemData.nmShortcut == NULL)
		{
		itemData.nmShortcut = new JString(str);
		assert( itemData.nmShortcut != NULL );
		itsTextItemData->SetElement(index, itemData);
		changed = kJTrue;
		}
	else if (!strEmpty)
		{
		*(itemData.nmShortcut) = str;
		changed = kJTrue;
		}
	else if (itemData.nmShortcut != NULL)
		{
		delete itemData.nmShortcut;
		itemData.nmShortcut = NULL;
		itsTextItemData->SetElement(index, itemData);
		changed = kJTrue;
		}

	if (changed)
		{
		itsNeedGeomRecalcFlag = kJTrue;

		JXWindow* window = itsMenu->GetWindow();
		window->ClearMenuShortcut(itsMenu, index);

		int key;
		JXKeyModifiers modifiers(itsMenu->GetDisplay());
		if (itemData.nmShortcut != NULL &&
			ParseNMShortcut(itemData.nmShortcut, &key, &modifiers) &&
			!window->InstallMenuShortcut(itsMenu, index, key, modifiers))
			{
			SetNMShortcut(index, NULL);		// don't display if not registered
			}
		}
}

/******************************************************************************
 ParseNMShortcut (private)

	Convert prepended "Ctrl-", "Shift-", "Meta-", "Alt-", "Super-",
	"Hyper-", and "Mod1-" through "Mod5-" into modifier flags.  If the rest
	of the string is a single character, then we return this in key.  We
	also translate the strings defined in kNMKeyConv because the actual
	characters can confuse the user.  (e.g. Use "Ctrl-dash" instead of
	"Ctrl--")  We also decode "F1" through "F35" to be the function keys
	that X provides.

	"Shift-" is allowed only for alphabetic and control characters because
	of differences between keyboards.

	If a modifier is not available, the shortcut is not installed.

	Note that it is a bad idea to use "return" as a shortcut if the window
	contains a multi-line text editor.  It is also a bad idea to use
	"backspace" and "delete" if the window contains a text input field.
	"Meta-tab" is not available, because JXWindow catches it.

 ******************************************************************************/

struct JXTMModifierConversion
{
	const JCharacter*	str;
	JSize				strLength;
	JXModifierKey		key;
};

static const JXTMModifierConversion kNMModConv[] =
{
	{ "Shift-", 6, kJXShiftKeyIndex   },
	{ "Ctrl-",  5, kJXControlKeyIndex },
	{ "Mod1-",  5, kJXMod1KeyIndex    },
	{ "Mod2-",  5, kJXMod2KeyIndex    },
	{ "Mod3-",  5, kJXMod3KeyIndex    },
	{ "Mod4-",  5, kJXMod4KeyIndex    },
	{ "Mod5-",  5, kJXMod5KeyIndex    },
	{ "Meta-",  5, kJXMetaKeyIndex    },
	{ "Alt-",   4, kJXAltKeyIndex     },
	{ "Super-", 6, kJXSuperKeyIndex   },
	{ "Hyper-", 6, kJXHyperKeyIndex   }
};

const JSize kNMModConvCount = sizeof(kNMModConv)/sizeof(JXTMModifierConversion);

struct JXTMKeySymConversion
{
	const JCharacter*	str;
	int					key;
};

static const JXTMKeySymConversion kNMKeyConv[] =
{
	{"dash",      '-'},
	{"minus",     '-'},
	{"plus",      '+'},
	{"period",    '.'},
	{"comma",     ','},
	{"return",    '\r'},
	{"tab",       '\t'},
	{"backspace", '\b'},
	{"delete",    kJForwardDeleteKey},
	{"insert",    XK_Insert},
	{"home",      XK_Home},
	{"end",       XK_End},
	{"page up",   XK_Page_Up},
	{"page down", XK_Page_Down}
};

const JSize kNMKeyConvCount = sizeof(kNMKeyConv)/sizeof(JXTMKeySymConversion);

JBoolean
JXTextMenuData::ParseNMShortcut
	(
	JString*		str,
	int*			key,
	JXKeyModifiers*	modifiers
	)
{
JIndex i;

	JString keyStr(*str);
	modifiers->Clear();

	// decode modifiers

	while (1)
		{
		JBoolean found = kJFalse;
		for (i=0; i<kNMModConvCount; i++)
			{
			if (keyStr.BeginsWith(kNMModConv[i].str) &&
				keyStr.GetLength() > kNMModConv[i].strLength)
				{
				const JXModifierKey key = JXMenu::AdjustNMShortcutModifier(kNMModConv[i].key);
				if (!modifiers->Available(key))
					{
					return kJFalse;
					}

				if (key != kNMModConv[i].key)
					{
					AdjustNMShortcutString(str, i, key);
					}

				keyStr.RemoveSubstring(1, kNMModConv[i].strLength);
				modifiers->SetState(key, kJTrue);
				found = kJTrue;
				break;
				}
			}
		if (!found)
			{
			break;
			}
		}

	// translate known name to single character

	for (i=0; i<kNMKeyConvCount; i++)
		{
		if (JStringCompare(keyStr, kNMKeyConv[i].str, kJFalse) == 0)
			{
			const int k = kNMKeyConv[i].key;
			if (0 < k && k <= (int) UCHAR_MAX)
				{
				const JCharacter s[2] = { k, '\0' };
				keyStr = s;
				break;	// extra processing below
				}
			else
				{
				*key = k;
				return kJTrue;
				}
			}
		}

	// check for single character

	const int c1 = (unsigned char) keyStr.GetFirstCharacter();
	if (keyStr.GetLength() == 1)
		{
		if (!isalpha(c1) && !iscntrl(c1))
			{
			// can't complain because of menu_strings file
			modifiers->SetState(kJXShiftKeyIndex, kJFalse);
			}

		*key = tolower(c1);
		return kJTrue;
		}

	// check for function key

	JSize fnIndex;
	if (c1 == 'F' &&
		JString::ConvertToUInt(keyStr.GetCString()+1, &fnIndex) &&
		1 <= fnIndex && fnIndex <= 35)
		{
		*key = XK_F1 + fnIndex-1;
		return kJTrue;
		}

	// give up

	return kJFalse;
}

/******************************************************************************
 AdjustNMShortcutString (private)

 ******************************************************************************/

void
JXTextMenuData::AdjustNMShortcutString
	(
	JString*			str,
	const JIndex		origKeyIndex,
	const JXModifierKey	newKey
	)
{
	JBoolean found     = kJFalse;
	JIndex newKeyIndex = 0;

	for (JIndex i=0; i<kNMModConvCount; i++)
		{
		if (newKey == kNMModConv[i].key)
			{
			newKeyIndex = i;
			found       = kJTrue;
			break;
			}
		}

	if (found)
		{
		JIndex i = 1;
		while (str->LocateNextSubstring(kNMModConv[origKeyIndex].str, &i))
			{
			str->ReplaceSubstring(i, i + kNMModConv[origKeyIndex].strLength-1,
								  kNMModConv[newKeyIndex].str);
			i += kNMModConv[newKeyIndex].strLength;
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
	const JBoolean	show
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	if (itemData.separator != show)
		{
		itemData.separator = show;
		itsTextItemData->SetElement(index, itemData);

		itsNeedGeomRecalcFlag = kJTrue;
		}
}

/******************************************************************************
 ItemShortcutsChanged (virtual protected)

	Derived classes can override this to update underlining, etc.

	Note that shortcuts can be NULL.

 ******************************************************************************/

void
JXTextMenuData::ItemShortcutsChanged
	(
	const JIndex	index,
	const JString*	shortcuts
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	itemData.ulIndex =
		JXWindow::GetULShortcutIndex(*(itemData.text), shortcuts);
	itsTextItemData->SetElement(index, itemData);
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
	const JBoolean hasCheckboxes = HasCheckboxes();
	const JBoolean hasSubmenus   = HasSubmenus();

	if (itsNeedGeomRecalcFlag)
		{
		itsNeedGeomRecalcFlag = kJFalse;
		itsMaxImageWidth      = 1;
		itsMaxTextWidth       = 1;
		itsMaxShortcutWidth   = JXMenuTable::kSubmenuColWidth;
		itsHasNMShortcutsFlag = kJFalse;
		itsItemHeights->RemoveAll();

		const JSize itemCount = itsTextItemData->GetElementCount();
		for (JIndex i=1; i<=itemCount; i++)
			{
			const TextItemData itemData = itsTextItemData->GetElement(i);
			JCoordinate h =
				(itsCompressHeightFlag && !hasCheckboxes && !hasSubmenus) ?
				0 : JXMenuTable::kMinRowHeight;

			if (itemData.text != NULL)
				{
				const JCoordinate th =
					itsFontMgr->GetLineHeight(itemData.fontID, itemData.fontSize,
											  itemData.fontStyle);
				h = JMax(h, th);
				const JCoordinate tw = 2*JXTextMenuTable::kHMarginWidth +
					itsFontMgr->GetStringWidth(itemData.fontID, itemData.fontSize,
											   itemData.fontStyle, *(itemData.text));
				itsMaxTextWidth = JMax(itsMaxTextWidth, tw);
				}

			if (itemData.image != NULL)
				{
				h = JMax(h, (itemData.image)->GetHeight());
				itsMaxImageWidth = JMax(itsMaxImageWidth, (itemData.image)->GetWidth());
				}

			if (itemData.nmShortcut != NULL)
				{
				JFontStyle style;
				itsHasNMShortcutsFlag = kJTrue;
				const JCoordinate th =
					itsFontMgr->GetLineHeight(itemData.fontID, itemData.fontSize, style);
				h = JMax(h, th);
				const JCoordinate tw = JXTextMenuTable::kHNMSMarginWidth +
					JXTextMenuTable::kHMarginWidth +
					itsFontMgr->GetStringWidth(itemData.fontID, itemData.fontSize,
											   style, *(itemData.nmShortcut));
				itsMaxShortcutWidth = JMax(itsMaxShortcutWidth, tw);
				}

			h += 2*(JXTextMenuTable::kHilightBorderWidth + 1);
			if (i < itemCount && itemData.separator)
				{
				h += JXTextMenuTable::kSeparatorHeight;
				}

			table->SetRowHeight(i,h);
			itsItemHeights->AppendElement(h);
			}
		}
	else
		{
		JRunArrayIterator<JCoordinate> iter(itsItemHeights);
		JCoordinate h;
		JIndex i=0;
		while(iter.Next(&h))
			{
			i++;
			table->SetRowHeight(i,h);
			}
		}

	// set the column widths

	if (hasCheckboxes)
		{
		table->SetColWidth(1, JXMenuTable::kCheckboxColWidth +
							  JXTextMenuTable::kHilightBorderWidth);
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
		(itsFontMgr->GetLineHeight(itsDefFontID, itsDefFontSize, itsDefFontStyle)
		+ 2*(JXTextMenuTable::kHilightBorderWidth + 1));
	table->SetDefaultRowHeight(scrollStep);
}

#define JTemplateType JXTextMenuData::TextItemData
#include <JArray.tmpls>
#undef JTemplateType
