/******************************************************************************
 jXMenuUtil.cpp

	Utilities for parsing menu data.

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jXMenuUtil.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <X11/keysym.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 JXParseMenuItemStr

	%d = disabled, %l = separator line after item,
	%b = checkbox, %r = radio button,
	%h<chars> = specify shortcuts, %k<chars> = specify non-menu shortcut string,
	%i<chars> = internal identifier

 ******************************************************************************/

static JString
jGetOpValue
	(
	const JString& op
	)
{
	JStringIterator iter(op);
	iter.SkipNext();
	iter.BeginMatch();
	iter.MoveTo(JStringIterator::kStartAtEnd, 0);
	JString result = iter.FinishMatch().GetString();
	result.TrimWhitespace();
	return result;
}

void
JXParseMenuItemStr
	(
	JString*			text,
	bool*				isActive,
	bool*				hasSeparator,
	JXMenu::ItemType*	type,
	JString*			shortcuts,
	JString*			nmShortcut,
	JString*			id
	)
{
	*isActive     = true;
	*hasSeparator = false;
	*type         = JXMenu::kPlainType;

	shortcuts->Clear();
	nmShortcut->Clear();
	id->Clear();

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	text->Split("%", &list);
	text->Set(*list.GetFirstItem());
	text->TrimWhitespace();

	const JSize count = list.GetItemCount();
	for (JIndex i=2; i<=count; i++)
	{
		const JString* op        = list.GetItem(i);
		const JUtf8Character opc = op->GetFirstCharacter().ToLower();
		if (opc == 'd')
		{
			*isActive = false;
		}
		else if (opc == 'l')
		{
			*hasSeparator = true;
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
			*shortcuts = jGetOpValue(*op);
			shortcuts->TrimWhitespace();
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
 JXParseNMShortcut

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
	{ "Meta-",  5, kJXMetaKeyIndex    },	// index = kMacOSModifierCount
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

const JSize kMacOSModifierCount = 3;

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

struct JXTMNameConversion
{
	const JUtf8Byte* name;
	const JUtf8Byte* str;
};

static const JXTMNameConversion kNMNameConv[] =
{
	{"dash",   "\xE2\x80\x93"},
	{"minus",  "\xE2\x80\x93"},
	{"plus",   "+"},
	{"period", "."},
	{"comma",  ","}
};

bool
JXParseNMShortcut
	(
	JString*		str,
	int*			key,
	JXKeyModifiers*	modifiers,
	const bool		useMacOSSymbols
	)
{
	JString keyStr(*str);
	modifiers->Clear();

	// decode modifiers

	JStringIterator iter(&keyStr);
	while (true)
	{
		bool found = false;
		for (const auto& conv : kNMModConv)
		{
			if (keyStr.StartsWith(conv.str) &&
				keyStr.GetByteCount() > conv.strLength)
			{
				const JXModifierKey mkey = JXAdjustNMShortcutModifier(conv.key);
				if (!modifiers->Available(mkey))
				{
					return false;
				}

				if (mkey != conv.key)
				{
					JXAdjustNMShortcutString(str, conv.str, mkey);
				}

				iter.SkipNext(conv.strLength);
				iter.RemoveAllPrev();
				modifiers->SetState(mkey, true);
				found = true;
				break;
			}
		}
		if (!found)
		{
			break;
		}
	}

	iter.Invalidate();

	// nicer display for OS X

	if (useMacOSSymbols &&
		(modifiers->shift() || modifiers->control() || modifiers->meta()))
	{
		JStringIterator iter(str);

		for (JUnsignedOffset i=0; i<kMacOSModifierCount; i++)
		{
			iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
			while (iter.Next(kNMModConv[i].str))
			{
				iter.RemoveLastMatch();
			}
		}

		iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
		const bool ok = iter.Next(keyStr);
		assert( ok );
		iter.RemoveLastMatch();
		iter.Invalidate();

		if (modifiers->shift())
		{
			str->Append("\xE2\x87\xA7");
		}

		if (modifiers->control())
		{
			str->Append("^");	// \xE2\x8C\x83
		}

		if (modifiers->meta())
		{
			str->Append("\xE2\x8C\x98");
		}

		bool found = false;
		for (const auto& conv : kNMNameConv)
		{
			if (keyStr == conv.name)
			{
				str->Append(conv.str);
				found = true;
				break;
			}
		}

		if (!found)
		{
			str->Append(keyStr);
		}
	}

	// translate known name to single character

	for (const auto& conv : kNMKeyConv)
	{
		if (JString::Compare(keyStr, conv.str, JString::kIgnoreCase) == 0)
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
				return true;
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
			modifiers->SetState(kJXShiftKeyIndex, false);
		}

		*key = c1.ToLower().GetBytes()[0];
		return true;
	}

	// check for function key

	JSize fnIndex;
	if (c1 == 'F' &&
		JString::ConvertToUInt(keyStr.GetBytes()+1, &fnIndex) &&
		1 <= fnIndex && fnIndex <= 35)
	{
		*key = XK_F1 + fnIndex-1;
		return true;
	}

	// give up

	return false;
}

/******************************************************************************
 JXAdjustNMShortcutModifier

 ******************************************************************************/

JXModifierKey
JXAdjustNMShortcutModifier
	(
	const JXModifierKey key
	)
{
	if (JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle)
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
 JXAdjustNMShortcutString

 ******************************************************************************/

void
JXAdjustNMShortcutString
	(
	JString*			str,
	const JUtf8Byte*	origStr,
	const JXModifierKey	newKey
	)
{
	bool found     = false;
	JIndex newKeyIndex = 0;

	for (JUnsignedOffset i=0; i<kNMModConvCount; i++)
	{
		if (newKey == kNMModConv[i].key)
		{
			newKeyIndex = i;
			found       = true;
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
