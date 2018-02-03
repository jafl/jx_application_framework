/******************************************************************************
 cbmUtil.cpp

	Functions used by both Code Crusader and Code Medic.

	Copyright (C) 1999 John Lindal.

 ******************************************************************************/

#include "cbmUtil.h"
#include "CBFnMenuUpdater.h"
#include "CBCStyler.h"
#include "CBJavaStyler.h"
#include "CBHTMLStyler.h"

#ifdef CODE_CRUSADER
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#elif defined CODE_MEDIC
#include "cmGlobals.h"
#endif

#include <JXDisplay.h>
#include <JXInputField.h>
#include <JXColormap.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

// shared prefs

static const JCharacter* kSharedPrefsFileName = ".jxcb/medic_shared_prefs";

const JFileVersion kCurrentSharedPrefsVersion = 6;

// version  6 adds CBHTMLStyler data and PHP file suffixes
// version  5 adds CBJavaStyler data
// version  4 adds Fortran and Java file suffixes
// version  3 adds WillIncludeNamespace()
// version  2 adds kRightMarginColorIndex to color list
// version  1 adds WillPackFnNames()

// Prototypes

JBoolean	CBMGetSharedPrefsFileName(JString* fileName);

#if defined CODE_CRUSADER

/******************************************************************************
 CBMWriteSharedPrefs

 ******************************************************************************/

void
CBMWriteSharedPrefs
	(
	const JBoolean replace
	)
{
JIndex i;

	JString fileName;
	if (!CBMGetSharedPrefsFileName(&fileName))
		{
		return;
		}

	if (!replace && JFileExists(fileName))
		{
		std::ifstream input(fileName);

		JFileVersion vers;
		input >> vers;
		if (vers >= kCurrentSharedPrefsVersion)
			{
			return;
			}
		}

	JString p,n;
	JSplitPathAndName(fileName, &p, &n);
	if (JCreateDirectory(p) != kJNoError)
		{
		return;
		}

	// don't replace newer prefs

	if (JFileExists(fileName))
		{
		std::ifstream input(fileName);
		JFileVersion vers;
		input >> vers;
		if (vers > kCurrentSharedPrefsVersion)
			{
			return;
			}
		}

	std::ofstream output(fileName);
	output << kCurrentSharedPrefsVersion;

	// font

	CBPrefsManager* prefsMgr = CBGetPrefsManager();

	JString fontName;
	JSize fontSize;
	prefsMgr->GetDefaultFont(&fontName, &fontSize);
	output << ' ' << fontName << ' ' << fontSize;

	// editor settings

	CBTextDocument* doc = jnew CBTextDocument;
	assert( doc != NULL );

	CBTextEditor* te         = doc->GetTextEditor();
	CBFnMenuUpdater* updater = CBMGetFnMenuUpdater();

	output << ' ' << te->GetTabCharCount();
	output << ' ' << updater->WillSortFnNames();
	output << ' ' << updater->WillIncludeNamespace();
	output << ' ' << updater->WillPackFnNames();
	output << ' ' << doc->WillOpenComplFileOnTop();

	doc->Close();
	doc = NULL;

	// default text colors

	output << ' ' << (long) CBPrefsManager::kColorCount;

	JColormap* colormap = ((JXGetApplication())->GetCurrentDisplay())->GetColormap();
	for (i=1; i<=CBPrefsManager::kColorCount; i++)
		{
		output << ' ' << colormap->GetRGB(prefsMgr->GetColor(i));
		}

	// stylers

	output << ' ';
	(CBCStyler::Instance())->WriteForSharedPrefs(output);

	output << ' ';
	(CBJavaStyler::Instance())->WriteForSharedPrefs(output);

	output << ' ';
	(CBHTMLStyler::Instance())->WriteForSharedPrefs(output);

	// File suffixes

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	prefsMgr->GetFileSuffixes(kCBCSourceFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kCBCHeaderFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kCBFortranFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kCBJavaSourceFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kCBPHPFT, &suffixList);
	output << ' ' << suffixList;
}

#elif defined CODE_MEDIC

/******************************************************************************
 CBMReadSharedPrefs

 ******************************************************************************/

JBoolean
CBMReadSharedPrefs
	(
	JString*			fontName,
	JSize*				fontSize,
	JSize*				tabCharCount,
	JBoolean*			sortFnNames,
	JBoolean*			includeNS,
	JBoolean*			packFnNames,
	JBoolean*			openComplFileOnTop,
	const JSize			userColorCount,
	JRGB				colorList[],
	JPtrArray<JString>*	cSourceSuffixList,
	JPtrArray<JString>*	cHeaderSuffixList,
	JPtrArray<JString>*	fortranSuffixList,
	JPtrArray<JString>*	javaSuffixList,
	JPtrArray<JString>*	phpSuffixList
	)
{
JIndex i;

	JString fileName;
	if (!CBMGetSharedPrefsFileName(&fileName))
		{
		return kJFalse;
		}

	std::ifstream input(fileName);
	if (!input.good())
		{
		return kJFalse;
		}

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSharedPrefsVersion)
		{
		return kJFalse;
		}

	// font

	input >> *fontName >> *fontSize;

	// editor settings

	input >> *tabCharCount >> *sortFnNames;

	*includeNS = kJFalse;
	if (vers >= 3)
		{
		input >> *includeNS;
		}

	*packFnNames = kJFalse;
	if (vers >= 1)
		{
		input >> *packFnNames;
		}

	input >> *openComplFileOnTop;

	// default text colors

	JSize colorCount;
	input >> colorCount;

	assert( userColorCount == 6 );
	if (vers < 2)
		{
		assert( colorCount == 5 );
		JColormap* colormap = (CBCStyler::Instance())->GetColormap();
		colorList[5]        = colormap->GetRGB(colormap->GetGrayColor(70));
		}
	else
		{
		assert( colorCount == userColorCount );
		}

	for (i=0; i<colorCount; i++)
		{
		input >> colorList[i];
		}

	// stylers

	(CBCStyler::Instance())->ReadFromSharedPrefs(input);

	if (vers >= 5)
		{
		(CBJavaStyler::Instance())->ReadFromSharedPrefs(input);
		}

	if (vers >= 6)
		{
		(CBHTMLStyler::Instance())->ReadFromSharedPrefs(input);
		}

	// File suffixes

	input >> *cSourceSuffixList >> *cHeaderSuffixList;

	if (vers >= 4)
		{
		input >> *fortranSuffixList >> *javaSuffixList;
		}
	else
		{
		fortranSuffixList->DeleteAll();
		javaSuffixList->DeleteAll();
		}

	if (vers >= 6)
		{
		input >> *phpSuffixList;
		}
	else
		{
		phpSuffixList->DeleteAll();
		}

	return kJTrue;
}

#endif

/******************************************************************************
 CBMGetSharedPrefsFileName

	Returns kJTrue if the user has a home directory.

 ******************************************************************************/

JBoolean
CBMGetSharedPrefsFileName
	(
	JString* fileName
	)
{
	if (JGetPrefsDirectory(fileName))
		{
		*fileName = JCombinePathAndName(*fileName, kSharedPrefsFileName);
		return kJTrue;
		}
	else
		{
		fileName->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 CBMParseEditorOptions

	If we ever support indent width separate from tab width:
		emacs: c-basic-offset
		vi:    shiftwidth= (sw)

 ******************************************************************************/

#define EMACS_TAB_WIDTH "tab-width:[ \t]*([0-9]+)"
#define EMACS_TAB_MODE  "indent-tabs-mode:[ \t]*(t|nil)"
#define EMACS_TOP_START "^/\\*+[ \t]*-\\*-.*[ \t]+"
#define EMACS_TOP_END   ".*-\\*-[ \t]*\\*+/"
#define EMACS_START     "^ \\* Local variables:[ \t]*\n(?: \\* .+\n)* \\* "
#define VI_START        "(?:ex|vim?):[ \t]*set.*[ \t]+"
#define VI_END          "(?:[ \t]+.*)?:"

static JRegex emacsTopTabWidthOption = EMACS_TOP_START EMACS_TAB_WIDTH EMACS_TOP_END;
static JRegex emacsTopTabModeOption  = EMACS_TOP_START EMACS_TAB_MODE  EMACS_TOP_END;
static JRegex emacsTabWidthOption    = EMACS_START EMACS_TAB_WIDTH;
static JRegex emacsTabModeOption     = EMACS_START EMACS_TAB_MODE;
static JRegex viTabWidthOption       = VI_START "(?:tabstop|ts)=([0-9]+)" VI_END;
static JRegex viAutoIndentOption     = VI_START "(autoindent|noautoindent|ai|noai)" VI_END;

void
CBMParseEditorOptions
	(
	const JString&	text,
	JBoolean*		setTabWidth,
	JSize*			tabWidth,
	JBoolean*		setTabMode,
	JBoolean*		tabInsertsSpaces,
	JBoolean*		setAutoIndent,
	JBoolean*		autoIndent
	)
{
	emacsTopTabWidthOption.SetCaseSensitive(kJFalse);
	emacsTopTabWidthOption.SetSingleLine(kJTrue);
	emacsTopTabModeOption.SetCaseSensitive(kJFalse);
	emacsTopTabModeOption.SetSingleLine(kJTrue);
	emacsTabWidthOption.SetCaseSensitive(kJFalse);
	emacsTabModeOption.SetCaseSensitive(kJFalse);

	*setTabWidth   = kJFalse;
	*setTabMode    = kJFalse;
	*setAutoIndent = kJFalse;

	JArray<JIndexRange> matchList;
	if (emacsTopTabWidthOption.Match(text, &matchList) ||
		emacsTabWidthOption.Match(text, &matchList)    ||
		viTabWidthOption.Match(text, &matchList))
		{
		const JString s = text.GetSubstring(matchList.GetElement(2));
		*setTabWidth    = s.ConvertToUInt(tabWidth);
		}

	if (emacsTopTabModeOption.Match(text, &matchList) ||
		emacsTabModeOption.Match(text, &matchList))
		{
		const JString s   = text.GetSubstring(matchList.GetElement(2));
		*setTabMode       = kJTrue;
		*tabInsertsSpaces = JI2B(s == "nil");
		}

	if (viAutoIndentOption.Match(text, &matchList))
		{
		const JString s = text.GetSubstring(matchList.GetElement(2));
		*setAutoIndent  = kJTrue;
		*autoIndent     = !s.BeginsWith("no");
		}
}

/******************************************************************************
 CBMScrollForDefinition

	Assumes that the line of the function's definition is selected.

 ******************************************************************************/

static JRegex scriptCommentPattern = "^(\n|[ \t]*#)";
static JRegex makeCommentPattern   = "^(\n|[ \t]*(#|\\.PHONY))";
static JRegex antCommentPattern    = "^(\n|[ \t]*<!--)";
static JRegex lispCommentPattern   = "^(\n|[ \t]*;;)";
static JRegex aspCommentPattern    = "^(\n|[ \t]*')";
static JRegex sqlCommentPattern    = "^(\n|[ \t]*--)";

inline void
CBMIncludeScriptComments
	(
	JXTEBase*		te,
	const JRegex&	pattern,
	JIndex*			charIndex
	)
{
	const JString& text = te->GetText();
	while (*charIndex > 1)
		{
		const JIndex line = te->GetLineForChar(*charIndex-1);
		const JIndex i    = te->GetLineStart(line);
		const JIndex j    = te->GetLineEnd(line);	// regex code is slow

		*charIndex = i;		// point to line above before comment block
		if (!pattern.MatchWithin(text, JIndexRange(i, j)))
			{
			break;
			}
		}
}

void
CBMScrollForDefinition
	(
	JXTEBase*			te,
	const CBLanguage	lang
	)
{
	const JString& text         = te->GetText();
	const JIndex startIndex     = te->GetInsertionIndex();
	const JIndex startLineIndex = te->GetLineForChar(startIndex);

	JIndex charIndex = startIndex;
	if (lang == kCBCLang          ||
		lang == kCBCSharpLang     ||
		lang == kCBJavaLang       ||
		lang == kCBJavaScriptLang ||
		lang == kCBLexLang        ||
		lang == kCBMatlabLang     ||
		lang == kCBBisonLang      ||
		lang == kCBPascalLang     ||
		lang == kCBPHPLang        ||
		lang == kCBAdobeFlexLang  ||
		lang == kCBVeraLang)
		{
		// search backwards for closing brace (end of function) or semicolon (declaration)

		while (charIndex > 1)
			{
			const JCharacter c = text.GetCharacter(charIndex);
			if (c == '}' || c == ';')
				{
				break;
				}
			charIndex--;
			}
		}
	else if (lang == kCBAWKLang         ||
			 lang == kCBPerlLang        ||
			 lang == kCBPythonLang      ||
			 lang == kCBRubyLang        ||
			 lang == kCBBourneShellLang ||
			 lang == kCBCShellLang      ||
			 lang == kCBTCLLang         ||
			 lang == kCBVimLang         ||
			 lang == kCBSQLLang)
		{
		// search backwards for line that is neither comment nor empty

		CBMIncludeScriptComments(te, scriptCommentPattern, &charIndex);
		}
	else if (lang == kCBMakeLang)
		{
		// search backwards for line that is neither comment nor empty

		makeCommentPattern.SetCaseSensitive(kJFalse);
		CBMIncludeScriptComments(te, makeCommentPattern, &charIndex);
		}
	else if (lang == kCBAntLang)
		{
		// search backwards for line that is neither comment nor empty

		CBMIncludeScriptComments(te, antCommentPattern, &charIndex);
		}
	else if (lang == kCBLispLang)
		{
		// search backwards for line that is neither comment nor empty

		CBMIncludeScriptComments(te, lispCommentPattern, &charIndex);
		}
	else if (lang == kCBASPLang)
		{
		// search backwards for line that is neither comment nor empty

		CBMIncludeScriptComments(te, aspCommentPattern, &charIndex);
		}
	else if (lang == kCBSQLLang)
		{
		// search backwards for line that is neither comment nor empty

		CBMIncludeScriptComments(te, sqlCommentPattern, &charIndex);
		}

	// if we moved up without hitting the top of the file,
	// search down again for the next non-blank line

	JIndex lineIndex = te->GetLineForChar(charIndex);
	if (charIndex > 1)
		{
		while (lineIndex < startLineIndex)
			{
			lineIndex++;
			charIndex = te->GetLineStart(lineIndex);
			if (text.GetCharacter(charIndex) != '\n')
				{
				break;
				}
			}
		}

	// scroll to place this line at the top of the window,
	// if this shifts the original line up

	const JCoordinate lineTop = te->GetLineTop(lineIndex);
	if (lineTop > (te->GetAperture()).top)
		{
		te->ScrollTo(0, lineTop);
		}
}

/******************************************************************************
 CBMSelectLines

 ******************************************************************************/

void
CBMSelectLines
	(
	JTextEditor*		te,
	const JIndexRange&	origRange
	)
{
	JIndex lineIndex = te->CRLineIndexToVisualLineIndex(origRange.first);
	te->SelectLine(lineIndex);

	if (origRange.last > origRange.first)
		{
		lineIndex = te->CRLineIndexToVisualLineIndex(origRange.last);
		te->SetSelection(te->GetInsertionIndex(), te->GetLineEnd(lineIndex));
		}

	if (!te->WillBreakCROnly())
		{
		const JIndex charIndex = te->GetLineStart(lineIndex);
		te->SetSelection(te->GetInsertionIndex(), te->GetParagraphEnd(charIndex));
		}
}

/******************************************************************************
 CBMBalanceFromSelection

 ******************************************************************************/

void
CBMBalanceFromSelection
	(
	JXTEBase*			te,
	const CBLanguage	lang
	)
{
	JIndex selStart, selEnd;
	JBoolean hasSelection = te->GetSelection(&selStart, &selEnd);

	JIndex openIndex = te->GetInsertionIndex();

	// If a single grouping symbol is enclosed, balance it.

	const JString& text = te->GetText();
	if (hasSelection && selStart == selEnd)
		{
		const JCharacter c = text.GetCharacter(selStart);
		if (CBMIsOpenGroup(lang, c))
			{
			hasSelection = kJFalse;
			openIndex = selStart+1;
			}
		else if (CBMIsCloseGroup(lang, c))
			{
			hasSelection = kJFalse;
			openIndex = selEnd;
			}
		}
	else if (openIndex < text.GetLength() &&
			 CBMIsOpenGroup(lang, text.GetCharacter(openIndex)) &&
			 (openIndex == 1 ||
			  (!CBMIsOpenGroup(lang, openIndex-1) &&
			   !CBMIsCloseGroup(lang, openIndex-1))))
		{
		openIndex++;
		}
	else if (openIndex > 1 && CBMIsCloseGroup(lang, text.GetCharacter(openIndex-1)) &&
			 (openIndex > text.GetLength() ||
			  (!CBMIsOpenGroup(lang, text.GetCharacter(openIndex)) &&
			   !CBMIsCloseGroup(lang, text.GetCharacter(openIndex)))))
		{
		openIndex--;
		}

	JIndex closeIndex = openIndex;

	// balance groupers until the selection is enclosed or we get an error

	while (1)
		{
		const JBoolean foundOpen  = CBMBalanceBackward(lang, text, &openIndex);
		const JBoolean foundClose = CBMBalanceForward(lang, text,  &closeIndex);

		if (foundOpen && foundClose &&
			CBMIsMatchingPair(lang, text.GetCharacter(openIndex),
							  text.GetCharacter(closeIndex)))
			{
			if ((hasSelection &&
				 (selStart-1 < openIndex || closeIndex < selEnd+1 ||
				  (selStart-1 == openIndex && closeIndex == selEnd+1))) ||
				(!hasSelection && openIndex+1 == closeIndex))
				{
				closeIndex++;
				continue;
				}

			if (openIndex < closeIndex-1)
				{
				te->SetSelection(openIndex+1, closeIndex-1);
				}
			else
				{
				te->SetCaretLocation(closeIndex);
				}
			break;
			}
		else
			{
			(te->GetDisplay())->Beep();
			break;
			}
		}
}

/******************************************************************************
 Balancing (private)

	Forward:	search for the first unbalanced closing paren: ) } ]
				starting from the given index

	Backward:	search for the first unbalanced opening paren: ( { [
				starting from the character in front of the given index

	Returns the index of this character.

	By not using a C/C++ lexer to balance parentheses, we will display the
	wrong answer when parens are used inside character and string constants.

	However, we gain the ability to balance parens inside all other files,
	including letters to Grandma and other languages such as perl, sh, and
	FORTRAN.  We still make a few mistakes there, too, but the ability to
	get it right 99% of the time in all files is better than 100% of the
	time only in C/C++ files.

	In addition, this method is *much* faster because it is local, while a
	lexer is global because it has to start from the beginning of the file
	every time.

 ******************************************************************************/

JBoolean
CBMBalanceForward
	(
	const CBLanguage	lang,
	const JString&		str,
	JIndex*				index
	)
{
	JStack<JCharacter, JArray<JCharacter> > openList;

	for (JIndex i = *index; i <= str.GetLength(); i++)
		{
		const JCharacter c     = str.GetCharacter(i);
		const JBoolean isOpen  = CBMIsOpenGroup(lang, c);
		const JBoolean isClose = CBMIsCloseGroup(lang, c);

		if (isOpen)
			{
			openList.Push(c);
			}
		else if (isClose && openList.IsEmpty())
			{
			*index = i;
			return kJTrue;
			}
		else if (isClose)
			{
			const JCharacter c1 = openList.Pop();
			if (!CBMIsMatchingPair(lang, c1, c))
				{
				return kJFalse;
				}
			}
		}

	return kJFalse;
}

JBoolean
CBMBalanceBackward
	(
	const CBLanguage	lang,
	const JString&		str,
	JIndex*				index
	)
{
	assert( *index > 0 );

	JStack<JCharacter, JArray<JCharacter> > closeList;

	for (JIndex i = *index-1; i>=1; i--)
		{
		const JCharacter c     = str.GetCharacter(i);
		const JBoolean isOpen  = CBMIsOpenGroup(lang, c);
		const JBoolean isClose = CBMIsCloseGroup(lang, c);

		if (isClose)
			{
			closeList.Push(c);
			}
		else if (isOpen && closeList.IsEmpty())
			{
			*index = i;
			return kJTrue;
			}
		else if (isOpen)
			{
			const JCharacter c1 = closeList.Pop();
			if (!CBMIsMatchingPair(lang, c, c1))
				{
				return kJFalse;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 CBMIsCharacterInWord

	Returns true if the given character should be considered part of
	a word.  Our definition is [A-Za-z0-9_].

 ******************************************************************************/

JBoolean
CBMIsCharacterInWord
	(
	const JString&	text,
	const JIndex	charIndex
	)
{
	const JCharacter c = text.GetCharacter(charIndex);
	return JI2B( isalnum(c) || c == '_' );
}

#ifdef CODE_CRUSADER

/******************************************************************************
 CBMGetDocumentManager

 ******************************************************************************/

CBDocumentManager*
CBMGetDocumentManager()
{
	return CBGetDocumentManager();
}

/******************************************************************************
 CBMGetPrefsManager

 ******************************************************************************/

CBPrefsManager*
CBMGetPrefsManager()
{
	return CBGetPrefsManager();
}

/******************************************************************************
 CBMGetFnMenuUpdater

 ******************************************************************************/

CBFnMenuUpdater*
CBMGetFnMenuUpdater()
{
	return CBGetFnMenuUpdater();
}

#elif defined CODE_MEDIC

/******************************************************************************
 CBMGetDocumentManager

 ******************************************************************************/

CMSourceWindowManager*
CBMGetDocumentManager()
{
	assert( 0 );
	return NULL;
}

/******************************************************************************
 CBMGetPrefsManager

 ******************************************************************************/

CMPrefsManager*
CBMGetPrefsManager()
{
	return CMGetPrefsManager();
}

/******************************************************************************
 CBMGetFnMenuUpdater

 ******************************************************************************/

CBFnMenuUpdater*
CBMGetFnMenuUpdater()
{
	return CMGetFnMenuUpdater();
}

#endif

/******************************************************************************
 CBMGetStringList

	For use by dialogs.

 ******************************************************************************/

void
CBMGetStringList
	(
	JXInputField*		inputField,
	JPtrArray<JString>*	list
	)
{
	list->DeleteAll();

	JString text = inputField->GetText();
	text.TrimWhitespace();
	if (text.IsEmpty())
		{
		return;
		}

	text += " ";	// helps catch the last str

	// get rid of extra spaces

	JIndex i;
	while (text.LocateSubstring("\t", &i))
		{
		text.SetCharacter(i, ' ');
		}
	while (text.LocateSubstring("  ", &i))
		{
		text.RemoveSubstring(i+1,i+1);
		}

	// extract the strings

	while (!text.IsEmpty())
		{
		JIndex endIndex;
		const JBoolean found = text.LocateSubstring(" ", &endIndex);
		assert( found );

		JString* str = jnew JString(text.GetSubstring(1, endIndex-1));
		assert( str != NULL );
		list->Append(str);

		text.RemoveSubstring(1,endIndex);
		}
}

/******************************************************************************
 CBMGetSuffixList

	For use by dialogs.  Calls CBMGetStringList() and then checks that each
	string starts with a period.

 ******************************************************************************/

void
CBMGetSuffixList
	(
	JXInputField*		inputField,
	JPtrArray<JString>*	list
	)
{
	CBMGetStringList(inputField, list);

	const JSize count = list->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JString* suffix = list->GetElement(i);
		if (suffix->GetFirstCharacter() != '.')
			{
			suffix->PrependCharacter('.');
			}
		}
}

/******************************************************************************
 CBMSetStringList

	For use by dialogs.

 ******************************************************************************/

void
CBMSetStringList
	(
	JXInputField*				inputField,
	const JPtrArray<JString>&	list
	)
{
	JString text;

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (i > 1)
			{
			text += " ";
			}
		text += *(list.GetElement(i));
		}

	inputField->SetText(text);
}
