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

	BASE CLASS = JTEStyler, JPrefObject, virtual JBroadcaster

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBStylerBase.h"
#include "CBEditStylerDialog.h"
#include "cbmUtil.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JOrderedSetUtil.h>
#include <jAssert.h>

// setup information -- remember to increment shared prefs file version

const JFileVersion kCurrentSetupVersion = 0;

// JBroadcaster message types

const JCharacter* CBStylerBase::kWordListChanged = "WordListChanged::CBStylerBase";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStylerBase::CBStylerBase
	(
	const JSize				typeVersion,
	const JSize				typeCount,
	const JCharacter**		typeNames,
	const JCharacter*		editDialogTitle,
	const JPrefID&			prefID,
	const CBTextFileType	fileType
	)
	:
	JTEStyler(),
	JPrefObject(CBMGetPrefsManager(), prefID),
	itsColormap(((JXGetApplication())->GetCurrentDisplay())->GetColormap()),
	itsTypeNameVersion(typeVersion),
	itsTypeNameCount(typeCount),
	itsTypeNames(typeNames),
	itsFileType(fileType),
	itsDialogTitle(editDialogTitle),
	itsEditDialog(NULL)
{
	itsTypeStyles = new JArray<JFontStyle>;
	assert( itsTypeStyles != NULL );

	itsDefColor = (CBMGetPrefsManager())->GetColor(CBMPrefsManager::kTextColorIndex);

	JFontStyle style(itsDefColor);
	for (JIndex i=1; i<=typeCount; i++)
		{
		itsTypeStyles->AppendElement(style);
		}

	itsWordStyles = new JStringMap<JFontStyle>;
	assert( itsWordStyles != NULL );

	ListenTo(CBMGetPrefsManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBStylerBase::~CBStylerBase()
{
	delete itsTypeStyles;
	delete itsWordStyles;
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
	istream& input
	)
{
JIndex i;

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JBoolean active;
	input >> active;

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
	istream& input
	)
{
	JFontStyle style;

	input >> style.bold;
	input >> style.italic;
	input >> style.underlineCount;
	input >> style.strike;

	JRGB color;
	input >> color;
	style.color = itsColormap->GetColor(color);

	return style;
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBStylerBase::WritePrefs
	(
	ostream& output
	)
	const
{
JIndex i;

	output << kCurrentSetupVersion;
	output << ' ' << IsActive();

	// type styles

	const JSize typeCount = itsTypeStyles->GetElementCount();
	output << ' ' << typeCount;

	for (i=1; i<=typeCount; i++)
		{
		WriteStyle(output, itsTypeStyles->GetElement(i));
		}

	output << ' ' << itsTypeNameVersion;

	// word styles

	JArray<WordStyle> wordList;
	GetWordList(*itsWordStyles, &wordList, kJFalse);

	const JSize wordCount = wordList.GetElementCount();
	output << ' ' << wordCount;

	JString s;
	for (i=1; i<=wordCount; i++)
		{
		const WordStyle style = wordList.GetElement(i);

		s = style.key;
		output << ' ' << s;

		WriteStyle(output, style.value);
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
	ostream&			output,
	const JFontStyle&	style
	)
	const
{
	output << ' ' << style.bold;
	output << ' ' << style.italic;
	output << ' ' << style.underlineCount;
	output << ' ' << style.strike;

	output << ' ' << itsColormap->GetRGB(style.color);
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
	const JBoolean					sort
	)
	const
{
	wordList->RemoveAll();
	wordList->SetSortOrder(JOrderedSetT::kSortAscending);
	wordList->SetCompareFunction(CompareWords);

	JStringMapCursor<JFontStyle> cursor(&wordStyles);
	while (cursor.Next())
		{
		const WordStyle data(cursor.GetKey(), cursor.GetValue());
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

JOrderedSetT::CompareResult
CBStylerBase::CompareWords
	(
	const WordStyle& w1,
	const WordStyle& w2
	)
{
	const int result = JStringCompare(w1.key, w2.key, kJFalse);

	if (result < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (result == 0)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
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
	if (sender == CBMGetPrefsManager() &&
		message.Is(CBMPrefsManager::kTextColorChanged))
		{
		SetDefaultFontColor((CBMGetPrefsManager())->GetColor(CBMPrefsManager::kTextColorIndex));
		}

	else if (sender == itsEditDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			ExtractStyles();
			}
		(itsEditDialog->GetWindow())->WriteGeometry(&itsDialogGeom);
		itsEditDialog = NULL;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 SetDefaultFontColor (private)

	Called when user changes default font color.  We change all styles
	that have the same color as itsDefColor.

 ******************************************************************************/

void
CBStylerBase::SetDefaultFontColor
	(
	const JColorIndex color
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

		JArray<WordStyle> wordList;
		GetWordList(*itsWordStyles, &wordList, kJFalse);
		const JSize wordCount = wordList.GetElementCount();
		for (i=1; i<=wordCount; i++)
			{
			WordStyle data = wordList.GetElement(i);
			if (data.value.color == itsDefColor)
				{
				data.value.color = color;
				itsWordStyles->SetElement(data.key, data.value);
				}
			}

		itsDefColor = color;
		}
}

/******************************************************************************
 EditStyles

 ******************************************************************************/

void
CBStylerBase::EditStyles()
{
	assert( itsEditDialog == NULL );

	JArray<WordStyle> wordList;
	GetWordList(*itsWordStyles, &wordList, kJTrue);

	itsEditDialog = new CBEditStylerDialog(itsDialogTitle, IsActive(),
										   itsTypeNames, *itsTypeStyles,
										   wordList, itsFileType);
	assert( itsEditDialog != NULL );

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
	assert( itsEditDialog != NULL );

	JBoolean active;
	JArray<JFontStyle> newTypeStyles = *itsTypeStyles;
	JStringMap<JFontStyle> newWordStyles;
	itsEditDialog->GetData(&active, &newTypeStyles, &newWordStyles);
	if (active != IsActive() ||
		TypeStylesChanged(newTypeStyles) ||
		WordStylesChanged(newWordStyles))
		{
		itsEditDialog->GetData(&active, itsTypeStyles, itsWordStyles);

		SetActive(active);
		(CBMGetDocumentManager())->StylerChanged(this);

		Broadcast(WordListChanged(*itsWordStyles));
		}
}

/******************************************************************************
 TypeStylesChanged (private)

 ******************************************************************************/

JBoolean
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
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 WordStylesChanged (private)

 ******************************************************************************/

JBoolean
CBStylerBase::WordStylesChanged
	(
	const JStringMap<JFontStyle>& newWordStyles
	)
	const
{
	const JSize count = itsWordStyles->GetElementCount();
	if (newWordStyles.GetElementCount() != count)
		{
		return kJTrue;
		}
	else if (count == 0)
		{
		return kJFalse;
		}

	JArray<WordStyle> oldWordList(count);
	GetWordList(*itsWordStyles, &oldWordList, kJTrue);

	JArray<WordStyle> newWordList(count);
	GetWordList(newWordStyles, &newWordList, kJTrue);

	for (JIndex i=1; i<=count; i++)
		{
		const WordStyle w1 = oldWordList.GetElement(i);
		const WordStyle w2 = newWordList.GetElement(i);
		if (strcmp(w1.key, w2.key) != 0 || w1.value != w2.value)
			{
			return kJTrue;
			}
		}

	return kJFalse;
}
