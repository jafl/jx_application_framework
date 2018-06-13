/******************************************************************************
 JXTextMenuData.cpp

	Stores a string and an image (optional) for each menu item.

	BASE CLASS = JXMenuData

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXTextMenuData.h>
#include <JXTextMenu.h>
#include <JXTextMenuTable.h>
#include <JXWindow.h>
#include <JXImage.h>
#include <jXGlobals.h>
#include <jXKeysym.h>
#include <JFontManager.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

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
	itsMenu( menu ),
	itsDefaultFont( JFontManager::GetDefaultFont() )
{
	itsTextItemData = jnew JArray<TextItemData>;
	assert( itsTextItemData != nullptr );

	itsNeedGeomRecalcFlag = kJTrue;
	itsMaxImageWidth      = 1;
	itsMaxTextWidth       = 1;
	itsMaxShortcutWidth   = 1;
	itsHasNMShortcutsFlag = kJFalse;
	itsCompressHeightFlag = kJFalse;

	itsItemHeights = jnew JRunArray<JCoordinate>;
	assert( itsItemHeights != nullptr );
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
	JString* text = jnew JString(str);
	assert( text != nullptr );

	TextItemData itemData(text, itsDefaultFont);
	itsTextItemData->InsertElementAtIndex(index, itemData);

	JXMenuData::InsertItem(index, type, shortcuts, id);

	const JString* s;
	GetItemShortcuts(index, &s);
	itemData.ulIndex = JXWindow::GetULShortcutIndex(*(itemData.text), s);
	itsTextItemData->SetElement(index, itemData);

	(itsMenu->GetWindow())->MenuItemInserted(itsMenu, index);
	itsNeedGeomRecalcFlag = kJTrue;

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
	for (TextItemData itemData : *itsTextItemData)
		{
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
	jdelete (itemData->text);
	itemData->text = nullptr;

	jdelete (itemData->nmShortcut);
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
	const TextItemData itemData = itsTextItemData->GetElement(index);
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
	const JUtf8Byte* menuStr,
	const JUtf8Byte* idNamespace
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

	If idNamespace is not nullptr, the string database is searched for
	"<item ID>::<idNamespace>" and this is parsed for the item's text
	and shortcuts.

 ******************************************************************************/

void
JXTextMenuData::InsertMenuItems
	(
	const JIndex		startIndex,
	const JUtf8Byte*	menuStr,
	const JUtf8Byte*	idNamespace
	)
{
	JStringManager* strMgr = JGetStringManager();

	JPtrArray<JString> itemList(JPtrArrayT::kDeleteAll);
	JString(menuStr, kJFalse).Split("|", &itemList);

	const JSize itemCount = itemList.GetElementCount();
	JString itemText, shortcuts, nmShortcut, id, strID, id1;
	for (JIndex i=1; i<=itemCount; i++)
		{
		itemText = *(itemList.GetElement(i));

		JBoolean isActive, hasSeparator;
		JXMenu::ItemType type;
		ParseMenuItemStr(&itemText, &isActive, &hasSeparator,
						 &type, &shortcuts, &nmShortcut, &id);

		if (!JString::IsEmpty(idNamespace) && !id.IsEmpty())
			{
			strID  = id;
			strID += "::";
			strID += idNamespace;
			JString* itemText1;
			if (strMgr->GetElement(strID, &itemText1) && itemText1 != nullptr)
				{
				itemText = *itemText1;
				JBoolean isActive1, hasSeparator1;
				JXMenu::ItemType type1;
				ParseMenuItemStr(&itemText, &isActive1, &hasSeparator1,
								 &type1, &shortcuts, &nmShortcut, &id1);
				}
			}

		InsertItem(startIndex + i-1, itemText, type, shortcuts, nmShortcut, id);
		if (!isActive)
			{
			DisableItem(i);
			}
		if (hasSeparator)
			{
			ShowSeparatorAfter(i);
			}
		}
}

/******************************************************************************
 ParseMenuItemStr (private)

	%d = disabled, %l = separator line after item,
	%b = checkbox, %r = radio button,
	%h<chars> = specify shortcuts, %k<chars> = specify non-menu shortcut string,
	%i<chars> = internal identifier

 ******************************************************************************/

JString
jGetOpValue
	(
	const JString& op
	)
{
	JStringIterator iter(op);
	iter.SkipNext();
	iter.BeginMatch();
	iter.MoveTo(kJIteratorStartAtEnd, 0);
	JString result = iter.FinishMatch().GetString();
	result.TrimWhitespace();
	return result;
}

void
JXTextMenuData::ParseMenuItemStr
	(
	JString*			text,
	JBoolean*			isActive,
	JBoolean*			hasSeparator,
	JXMenu::ItemType*	type,
	JString*			shortcuts,
	JString*			nmShortcut,
	JString*			id
	)
	const
{
	*isActive     = kJTrue;
	*hasSeparator = kJFalse;
	*type         = JXMenu::kPlainType;

	shortcuts->Clear();
	nmShortcut->Clear();
	id->Clear();

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	text->Split("%", &list);
	text->Set(*(list.GetFirstElement()));
	text->TrimWhitespace();

	const JBoolean isOSX = JI2B( JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle );

	const JSize count = list.GetElementCount();
	for (JIndex i=2; i<=count; i++)
		{
		const JString* op        = list.GetElement(i);
		const JUtf8Character opc = op->GetFirstCharacter().ToLower();
		if (opc == 'd')
			{
			*isActive = kJFalse;
			}
		else if (opc == 'l')
			{
			*hasSeparator = kJTrue;
			}
		else if (opc == 'b')
			{
			*type = JXMenu::kCheckboxType;
			}
		else if (opc == 'r')
			{
			*type = JXMenu::kRadioType;
			}

		else if (opc == 'h' && shortcuts->IsEmpty())
			{
			if (!isOSX)
				{
				*shortcuts = jGetOpValue(*op);
				shortcuts->TrimWhitespace();
				}
			}
		else if (opc == 'h')
			{
			std::cerr << "Tried to use %h more than once in '" << *op << '\'' << std::endl;
			}

		else if (opc == 'k' && nmShortcut->IsEmpty())
			{
			*nmShortcut = jGetOpValue(*op);
			nmShortcut->TrimWhitespace();
			}
		else if (opc == 'k')
			{
			std::cerr << "Tried to use %k more than once in '" << *op << '\'' << std::endl;
			}

		else if (opc == 'i' && id->IsEmpty())
			{
			*id = jGetOpValue(*op);
			id->TrimWhitespace();
			}
		else if (opc == 'i')
			{
			std::cerr << "Tried to use %i more than once in '" << *op << '\'' << std::endl;
			}

		else
			{
			std::cerr << "Unsupported option %" << opc << " in '" << *op << '\'' << std::endl;
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
	TextItemData itemData = itsTextItemData->GetElement(index);
	itemData.font.SetName(name);
	itsTextItemData->SetElement(index, itemData);

	itsNeedGeomRecalcFlag = kJTrue;
}

void
JXTextMenuData::SetFontSize
	(
	const JIndex	index,
	const JSize		size
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	if (size != itemData.font.GetSize())
		{
		itemData.font.SetSize(size);
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
	if (style != itemData.font.GetStyle())
		{
		itemData.font.SetStyle(style);
		itsTextItemData->SetElement(index, itemData);

		itsNeedGeomRecalcFlag = kJTrue;
		}
}

void
JXTextMenuData::SetFont
	(
	const JIndex	index,
	const JFont&	font
	)
{
	TextItemData itemData = itsTextItemData->GetElement(index);
	itemData.font = font;
	itsTextItemData->SetElement(index, itemData);

	itsNeedGeomRecalcFlag = kJTrue;
}

/******************************************************************************
 Set default font

 ******************************************************************************/

void
JXTextMenuData::SetDefaultFontName
	(
	const JString&	name,
	const JBoolean	updateExisting
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
	const JBoolean	updateExisting
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
	const JBoolean		updateExisting
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
	const JBoolean	updateExisting
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
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		TextItemData itemData = itsTextItemData->GetElement(i);
		if (itemData.font == oldFont)
			{
			itemData.font = newFont;
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
	if (itemData.image != nullptr)
		{
		*image = itemData.image;
		return kJTrue;
		}
	else
		{
		*image = nullptr;
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
	if (image == nullptr)
		{
		ClearImage(index);
		return;
		}

	TextItemData itemData = itsTextItemData->GetElement(index);

	if (itemData.image == nullptr ||
		(itemData.image->GetBounds()) != image->GetBounds())
		{
		itsNeedGeomRecalcFlag = kJTrue;
		}

	if (itemData.ownsImage && image != itemData.image)
		{
		jdelete itemData.image;
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
	if (itemData.image != nullptr)
		{
		if (itemData.ownsImage)
			{
			jdelete itemData.image;
			}
		itemData.image = nullptr;
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
	if (itemData.nmShortcut != nullptr)
		{
		*str = itemData.nmShortcut;
		return kJTrue;
		}
	else
		{
		*str = nullptr;
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
	JFont*			font
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);

	if (itemData.nmShortcut != nullptr)
		{
		*str  = itemData.nmShortcut;
		*font = itemData.font;
		return kJTrue;
		}
	else
		{
		*str = nullptr;
		*font = itsMenu->GetFontManager()->GetDefaultFont();
		return kJFalse;
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
	TextItemData itemData = itsTextItemData->GetElement(index);

	const JBoolean strEmpty = str.IsEmpty();

	JBoolean changed = kJFalse;
	if (!strEmpty && itemData.nmShortcut == nullptr)
		{
		itemData.nmShortcut = jnew JString(str);
		assert( itemData.nmShortcut != nullptr );
		itsTextItemData->SetElement(index, itemData);
		changed = kJTrue;
		}
	else if (!strEmpty)
		{
		*(itemData.nmShortcut) = str;
		changed = kJTrue;
		}
	else if (itemData.nmShortcut != nullptr)
		{
		jdelete itemData.nmShortcut;
		itemData.nmShortcut = nullptr;
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
		if (itemData.nmShortcut != nullptr &&
			ParseNMShortcut(itemData.nmShortcut, &key, &modifiers) &&
			!window->InstallMenuShortcut(itsMenu, index, key, modifiers))
			{
			SetNMShortcut(index, JString::empty);	// don't display if not registered
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
	const JUtf8Byte*	str;
	JSize				strLength;
	JXModifierKey		key;
};

static const JXTMModifierConversion kNMModConv[] =
{
	{ "Shift-", 6, kJXShiftKeyIndex   },
	{ "Ctrl-",  5, kJXControlKeyIndex },
	{ "Meta-",  5, kJXMetaKeyIndex    },	// index = kOSXModifierCount
	{ "Mod1-",  5, kJXMod1KeyIndex    },
	{ "Mod2-",  5, kJXMod2KeyIndex    },
	{ "Mod3-",  5, kJXMod3KeyIndex    },
	{ "Mod4-",  5, kJXMod4KeyIndex    },
	{ "Mod5-",  5, kJXMod5KeyIndex    },
	{ "Alt-",   4, kJXAltKeyIndex     },
	{ "Super-", 6, kJXSuperKeyIndex   },
	{ "Hyper-", 6, kJXHyperKeyIndex   }
};

const JSize kNMModConvCount = sizeof(kNMModConv)/sizeof(JXTMModifierConversion);

const JSize kOSXModifierCount = 3;

struct JXTMKeySymConversion
{
	const JUtf8Byte*	str;
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

JBoolean
JXTextMenuData::ParseNMShortcut
	(
	JString*		str,
	int*			key,
	JXKeyModifiers*	modifiers
	)
{
	JString keyStr(*str);
	modifiers->Clear();

	// decode modifiers
	{
	JStringIterator iter(&keyStr);
	while (1)
		{
		JBoolean found = kJFalse;
		for (const JXTMModifierConversion& conv : kNMModConv)
			{
			if (keyStr.BeginsWith(conv.str) &&
				keyStr.GetByteCount() > conv.strLength)
				{
				const JXModifierKey key = JXMenu::AdjustNMShortcutModifier(conv.key);
				if (!modifiers->Available(key))
					{
					return kJFalse;
					}

				if (key != conv.key)
					{
					AdjustNMShortcutString(str, conv.str, key);
					}

				iter.SkipNext(conv.strLength);
				iter.RemoveAllPrev();
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
	}
	// nicer display for OS X

	if (itsMenu->GetDisplay()->IsOSX() &&
		(modifiers->shift() || modifiers->control() || modifiers->meta()))
		{
		JStringIterator iter(str);

		for (JIndex i=0; i<kOSXModifierCount; i++)
			{
			iter.MoveTo(kJIteratorStartAtBeginning, 0);
			while (iter.Next(kNMModConv[i].str))
				{
				iter.RemoveLastMatch();
				}
			}

		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		const JBoolean ok = iter.Next(keyStr);
		assert( ok );
		iter.RemoveLastMatch();

		if (modifiers->shift())
			{
			str->Append("\xE2\x87\xA7");
			}

		if (modifiers->control())
			{
			str->Append("\xE2\x8C\x83");
			}

		if (modifiers->meta())
			{
			str->Append("\xE2\x8C\x98");
			}

		str->Append(keyStr);
		}

	// translate known name to single character

	for (const JXTMKeySymConversion& conv : kNMKeyConv)
		{
		if (JString::Compare(keyStr, conv.str, kJFalse) == 0)
			{
			const int k = conv.key;
			if (0 < k && k <= (int) UCHAR_MAX)
				{
				const JUtf8Byte s[2] = { (char) k, '\0' };
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

	const JUtf8Character& c1 = keyStr.GetFirstCharacter();
	if (keyStr.GetCharacterCount() == 1)
		{
		if (!c1.IsAlpha() && !c1.IsControl())
			{
			// can't complain because of menu_strings file
			modifiers->SetState(kJXShiftKeyIndex, kJFalse);
			}

		*key = c1.ToLower().GetBytes()[0];
		return kJTrue;
		}

	// check for function key

	JSize fnIndex;
	if (c1 == 'F' &&
		JString::ConvertToUInt(keyStr.GetBytes()+1, &fnIndex) &&
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
	const JUtf8Byte*	origStr,
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

	if (!found)
		{
		return;
		}

	JStringIterator iter(str);
	while (iter.Next(origStr))
		{
		iter.ReplaceLastMatch(kNMModConv[newKeyIndex].str);
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

	Note that shortcuts can be nullptr.

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

			if (itemData.text != nullptr)
				{
				const JCoordinate th = itemData.font.GetLineHeight(itsMenu->GetFontManager());
				h                    = JMax(h, th);
				const JCoordinate tw = 2*JXTextMenuTable::kHMarginWidth +
					itemData.font.GetStringWidth(itsMenu->GetFontManager(), *(itemData.text));
				itsMaxTextWidth = JMax(itsMaxTextWidth, tw);
				}

			if (itemData.image != nullptr)
				{
				h = JMax(h, (itemData.image)->GetHeight());
				itsMaxImageWidth = JMax(itsMaxImageWidth, (itemData.image)->GetWidth());
				}

			if (itemData.nmShortcut != nullptr)
				{
				itsHasNMShortcutsFlag = kJTrue;
				JFont f = itemData.font;
				f.ClearStyle();
				const JCoordinate th = f.GetLineHeight(itsMenu->GetFontManager());
				h = JMax(h, th);
				const JCoordinate tw = JXTextMenuTable::kHNMSMarginWidth +
					JXTextMenuTable::kHMarginWidth +
					itemData.font.GetStringWidth(itsMenu->GetFontManager(), *(itemData.nmShortcut));
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
		itsDefaultFont.GetLineHeight(itsMenu->GetFontManager()) +
		2*(JXTextMenuTable::kHilightBorderWidth + 1);
	table->SetDefaultRowHeight(scrollStep);
}
