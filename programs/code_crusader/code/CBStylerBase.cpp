/******************************************************************************
 CBStylerBase.cpp

	Base class for all Code Crusader styler classes.  This stores an
	associative array of styles for arbitrary strings and an array of
	styles for token types.  It also handles reading, writing, and the
	dialog window for editing both these data structures.

	GetStyle() takes a token type and the text and looks up the appropriate
	style.

	Derived classes must override the following function:

		UpgradeTypeList
			Upgrade the given list of type styles from the given version
			to the latest version, so it matches the type names passed
			to the constructor.

	BASE CLASS = JSTStyler, JPrefObject, virtual JBroadcaster

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBStylerBase.h"
#include "CBEditStylerDialog.h"
#include "cbmUtil.h"
#include <JXWindow.h>
#include <JColorManager.h>
#include <jAssert.h>

// setup information -- remember to increment shared prefs file version

const JFileVersion kCurrentSetupVersion = 0;

// JBroadcaster message types

const JUtf8Byte* CBStylerBase::kWordListChanged = "WordListChanged::CBStylerBase";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStylerBase::CBStylerBase
	(
	const JSize				typeVersion,
	const JSize				typeCount,
	const JUtf8Byte**		typeNames,
	const JString&			editDialogTitle,
	const JPrefID&			prefID,
	const CBTextFileType	fileType
	)
	:
#ifndef CODE_CRUSADER_UNIT_TEST
	JPrefObject(CBMGetPrefsManager(), prefID),
#else
	JPrefObject(nullptr, prefID),
#endif
	itsTypeNameVersion(typeVersion),
	itsTypeNameCount(typeCount),
	itsTypeNames(typeNames),
	itsFileType(fileType),
	itsDialogTitle(editDialogTitle),
	itsEditDialog(nullptr)
{
	itsTypeStyles = jnew JArray<JFontStyle>(64);
	assert( itsTypeStyles != nullptr );

#ifndef CODE_CRUSADER_UNIT_TEST
	itsDefColor = CBMGetPrefsManager()->GetColor(CBMPrefsManager::kTextColorIndex);
#else
	itsDefColor = JColorManager::GetBlackColor();
#endif

	JFontStyle style(itsDefColor);
	for (JIndex i=1; i<=typeCount; i++)
		{
		itsTypeStyles->AppendElement(style);
		}

	itsWordStyles = jnew JStringMap<JFontStyle>(64);
	assert( itsWordStyles != nullptr );

#ifndef CODE_CRUSADER_UNIT_TEST
	ListenTo(CBMGetPrefsManager());
#endif
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBStylerBase::~CBStylerBase()
{
	jdelete itsTypeStyles;
	jdelete itsWordStyles;
}

/******************************************************************************
 GetStyle (protected)

	If word is not found in our associative array, we use the typeIndex.

 ******************************************************************************/

JFontStyle
CBStylerBase::GetStyle
	(
	const JIndex	typeIndex,
	const JString&	word
	)
{
	JFontStyle style;
	if (!GetWordStyle(word, &style))
		{
		style = GetTypeStyle(typeIndex);
		}

	return style;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBStylerBase::ReadPrefs
	(
	std::istream& input
	)
{
JIndex i;

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	bool active;
	input >> JBoolFromString(active);

	// type styles

	JArray<JFontStyle> typeStyles;

	JSize typeCount;
	input >> typeCount;

	for (i=1; i<=typeCount; i++)
		{
		typeStyles.AppendElement(ReadStyle(input));
		}

	JFileVersion typeListVers;
	input >> typeListVers;
	if (typeListVers > itsTypeNameVersion)
		{
		return;
		}

	SetActive(active);
	*itsTypeStyles = typeStyles;
	UpgradeTypeList(typeListVers, itsTypeStyles);
	assert( itsTypeStyles->GetElementCount() == itsTypeNameCount );

	// word styles

	itsWordStyles->RemoveAll();

	JSize wordCount;
	input >> wordCount;

	JString s;
	for (i=1; i<=wordCount; i++)
		{
		input >> s;
		itsWordStyles->SetElement(s, ReadStyle(input));
		}

	Broadcast(WordListChanged(*itsWordStyles));

	// dialog geometry

	input >> itsDialogGeom;
}

/******************************************************************************
 ReadStyle (private)

 ******************************************************************************/

JFontStyle
CBStylerBase::ReadStyle
	(
	std::istream& input
	)
{
	JFontStyle style;

	input >> JBoolFromString(style.bold);
	input >> JBoolFromString(style.italic);
	input >> style.underlineCount;
	input >> JBoolFromString(style.strike);

	JRGB color;
	input >> color;
	style.color = JColorManager::GetColorID(color);

	return style;
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBStylerBase::WritePrefs
	(
	std::ostream& output
	)
	const
{
JIndex i;

	output << kCurrentSetupVersion;
	output << ' ' << JBoolToString(IsActive());

	// type styles

	const JSize typeCount = itsTypeStyles->GetElementCount();
	output << ' ' << typeCount;

	for (i=1; i<=typeCount; i++)
		{
		WriteStyle(output, itsTypeStyles->GetElement(i));
		}

	output << ' ' << itsTypeNameVersion;

	// word styles

	output << ' ' << itsWordStyles->GetElementCount();

	JStringMapCursor<JFontStyle> cursor(itsWordStyles);
	while (cursor.Next())
		{
		output << ' ' << cursor.GetKey();
		WriteStyle(output, cursor.GetValue());
		}

	// dialog geometry

	output << ' ' << itsDialogGeom;
}

/******************************************************************************
 WriteStyle (private)

 ******************************************************************************/

void
CBStylerBase::WriteStyle
	(
	std::ostream&		output,
	const JFontStyle&	style
	)
	const
{
	output << ' ' << JBoolToString(style.bold);
	output << ' ' << JBoolToString(style.italic);
	output << ' ' << style.underlineCount;
	output << ' ' << JBoolToString(style.strike);

	output << ' ' << JColorManager::GetRGB(style.color);
}

/******************************************************************************
 GetWordList (private)

	Turns wordStyles into an array.

 ******************************************************************************/

void
CBStylerBase::GetWordList
	(
	const JStringMap<JFontStyle>&	wordStyles,
	JArray<WordStyle>*				wordList,
	const bool						sort
	)
	const
{
	wordList->RemoveAll();
	wordList->SetSortOrder(JListT::kSortAscending);
	wordList->SetCompareFunction(CompareWords);

	JStringMapCursor<JFontStyle> cursor(&wordStyles);
	while (cursor.Next())
		{
		const WordStyle data(jnew JString(cursor.GetKey()), cursor.GetValue());
		assert( data.key != nullptr );
		if (sort)
			{
			wordList->InsertSorted(data);
			}
		else
			{
			wordList->AppendElement(data);
			}
		}
}

/******************************************************************************
 CompareWords (static private)

 ******************************************************************************/

JListT::CompareResult
CBStylerBase::CompareWords
	(
	const WordStyle& w1,
	const WordStyle& w2
	)
{
	const int result = JString::Compare(*w1.key, *w2.key, JString::kIgnoreCase);

	if (result < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else if (result == 0)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBStylerBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
#ifndef CODE_CRUSADER_UNIT_TEST

	if (sender == CBMGetPrefsManager() &&
		message.Is(CBMPrefsManager::kTextColorChanged))
		{
		SetDefaultFontColor((CBMGetPrefsManager())->GetColor(CBMPrefsManager::kTextColorIndex));
		}

	else if (sender == itsEditDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info = dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			ExtractStyles();
			}
		itsEditDialog->GetWindow()->WriteGeometry(&itsDialogGeom);
		itsEditDialog = nullptr;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}

#endif
}

/******************************************************************************
 SetDefaultFontColor (private)

	Called when user changes default font color.  We change all styles
	that have the same color as itsDefColor.

 ******************************************************************************/

void
CBStylerBase::SetDefaultFontColor
	(
	const JColorID color
	)
{
JIndex i;

	if (color != itsDefColor)
		{
		for (i=1; i<=itsTypeNameCount; i++)
			{
			JFontStyle style = itsTypeStyles->GetElement(i);
			if (style.color == itsDefColor)
				{
				style.color = color;
				itsTypeStyles->SetElement(i, style);
				}
			}

		JStringMapCursor<JFontStyle> cursor(itsWordStyles);
		while (cursor.Next())
			{
			JFontStyle style = cursor.GetValue();
			if (style.color == itsDefColor)
				{
				style.color = color;
				itsWordStyles->SetOldElement(cursor.GetKey(), style);
				}
			}

		itsDefColor = color;
		}
}

/******************************************************************************
 EditStyles

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

void
CBStylerBase::EditStyles()
{
	assert( itsEditDialog == nullptr );

	JArray<WordStyle> wordList;
	GetWordList(*itsWordStyles, &wordList, true);

	itsEditDialog = jnew CBEditStylerDialog(itsDialogTitle, IsActive(),
										   itsTypeNames, *itsTypeStyles,
										   wordList, itsFileType);
	assert( itsEditDialog != nullptr );

	for (auto& ws : wordList)
		{
		jdelete ws.key;
		}

	JXWindow* window = itsEditDialog->GetWindow();
	window->ReadGeometry(itsDialogGeom);
	window->Deiconify();

	itsEditDialog->BeginDialog();
	ListenTo(itsEditDialog);
}

/******************************************************************************
 ExtractStyles (private)

 ******************************************************************************/

void
CBStylerBase::ExtractStyles()
{
	assert( itsEditDialog != nullptr );

	bool active;
	JArray<JFontStyle> newTypeStyles = *itsTypeStyles;
	JStringMap<JFontStyle> newWordStyles;
	itsEditDialog->GetData(&active, &newTypeStyles, &newWordStyles);
	if (active != IsActive() ||
		TypeStylesChanged(newTypeStyles) ||
		WordStylesChanged(newWordStyles))
		{
		itsEditDialog->GetData(&active, itsTypeStyles, itsWordStyles);

		SetActive(active);
		CBMGetDocumentManager()->StylerChanged(this);

		Broadcast(WordListChanged(*itsWordStyles));
		}
}

#endif

/******************************************************************************
 TypeStylesChanged (private)

 ******************************************************************************/

bool
CBStylerBase::TypeStylesChanged
	(
	const JArray<JFontStyle>& newTypeStyles
	)
	const
{
	const JSize count = itsTypeStyles->GetElementCount();
	assert( newTypeStyles.GetElementCount() == count );

	for (JIndex i=1; i<=count; i++)
		{
		if (itsTypeStyles->GetElement(i) != newTypeStyles.GetElement(i))
			{
			return true;
			}
		}

	return false;
}

/******************************************************************************
 WordStylesChanged (private)

 ******************************************************************************/

bool
CBStylerBase::WordStylesChanged
	(
	const JStringMap<JFontStyle>& newWordStyles
	)
	const
{
	const JSize count = itsWordStyles->GetElementCount();
	if (newWordStyles.GetElementCount() != count)
		{
		return true;
		}
	else if (count == 0)
		{
		return false;
		}

	JStringMapCursor<JFontStyle> cursor(itsWordStyles);
	JFontStyle newStyle;
	while (cursor.Next())
		{
		if (!newWordStyles.GetElement(cursor.GetKey(), &newStyle) ||
			newStyle != cursor.GetValue())
			{
			return true;
			}
		}

	return false;
}
