/******************************************************************************
 CBMacroManager.cpp

	Stores a list of macro strings and associated actions.  The action is
	performed after the string is typed followed by a tab.

	Macros are specified by plain text, not regular expressions, because
	each macro should have a unique name.  Anything more complicated should
	be done with regex search & replace.

	BASE CLASS = none

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBMacroManager.h"
#include "CBMacroSubstitute.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbmUtil.h"
#include <JColormap.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

static CBMacroSubstitute theSubst;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 no longer stores dialog geometry or column width

/******************************************************************************
 Constructor

 ******************************************************************************/

CBMacroManager::CBMacroManager()
{
	CBMacroManagerX();
}

// private

void
CBMacroManager::CBMacroManagerX()
{
	itsMacroList = new CBMacroList;
	assert( itsMacroList != NULL );
	itsMacroList->SetSortOrder(JOrderedSetT::kSortAscending);
	itsMacroList->SetCompareFunction(CompareMacros);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

CBMacroManager::CBMacroManager
	(
	const CBMacroManager& source
	)
{
	CBMacroManagerX();

	const JSize count = (source.itsMacroList)->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const MacroInfo oldInfo = (source.itsMacroList)->GetElement(i);

		MacroInfo newInfo(new JString(*(oldInfo.macro)),
						  new JString(*(oldInfo.script)));
		assert( newInfo.macro != NULL && newInfo.script != NULL );
		itsMacroList->AppendElement(newInfo);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBMacroManager::~CBMacroManager()
{
	itsMacroList->DeleteAll();
	delete itsMacroList;
}

/******************************************************************************
 Perform

	Returns kJTrue if it found a script and performed it.

 ******************************************************************************/

JBoolean
CBMacroManager::Perform
	(
	const JIndex	caretIndex,
	CBTextDocument*	doc
	)
{
	if (caretIndex <= 1)
		{
		return kJFalse;
		}

	const JString& text   = (doc->GetTextEditor())->GetText();
	const JIndex endIndex = caretIndex - 1;

	const JSize macroCount = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=macroCount; i++)
		{
		const MacroInfo info = itsMacroList->GetElement(i);
		const JSize macroLen = (info.macro)->GetLength();
		if (macroLen <= endIndex)
			{
			if (JCompareMaxN(text.GetCString() + endIndex - macroLen,
							 *(info.macro), macroLen) &&
				(macroLen == endIndex ||
				 !CBMIsCharacterInWord(text, endIndex - macroLen + 1) ||
				 !CBMIsCharacterInWord(text, endIndex - macroLen)))
				{
				Perform(*(info.script), doc);
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 Perform (static)

 ******************************************************************************/

void
CBMacroManager::Perform
	(
	const JString&	script,
	CBTextDocument*	doc
	)
{
	CBTextEditor* te = doc->GetTextEditor();
	te->DeactivateCurrentUndo();

	JBoolean onDisk;
	const JString fullName = JPrepArgForExec(doc->GetFullName(&onDisk));
	const JString path     = JPrepArgForExec(doc->GetFilePath());
	const JString fileName = JPrepArgForExec(doc->GetFileName());

	JString root, suffix;
	if (JSplitRootAndSuffix(doc->GetFileName(), &root, &suffix))
		{
		root = JPrepArgForExec(root);
		suffix.PrependCharacter('.');
		}

	const JIndex charIndex = te->GetInsertionIndex();
	const JIndex lineIndex = te->GetLineForChar(charIndex);
	const JIndex lineStart = te->GetLineStart(lineIndex);

	const JString lineIndexStr(lineIndex, JString::kBase10);

	JString lineStr;
	if (charIndex > lineStart)
		{
		lineStr = JPrepArgForExec((te->GetText()).GetSubstring(lineStart, charIndex-1));
		}

	theSubst.UndefineAllVariables();
	theSubst.DefineVariable("f", fullName);
	theSubst.DefineVariable("p", path);
	theSubst.DefineVariable("n", fileName);
	theSubst.DefineVariable("r", root);
	theSubst.DefineVariable("s", suffix);
	theSubst.DefineVariable("l", lineIndexStr);
	theSubst.DefineVariable("t", lineStr);

	JString s = script;
	theSubst.Substitute(&s);

	JXKeyModifiers modifiers(te->GetDisplay());

	const JSize length = s.GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		te->JXTEBase::HandleKeyPress((unsigned char) s.GetCharacter(i), modifiers);
		}
}

/******************************************************************************
 HighlightErrors (static)

 ******************************************************************************/

void
CBMacroManager::HighlightErrors
	(
	const JString&		script,
	const JColormap*	colormap,
	JRunArray<JFont>*	styles
	)
{
	const JFont f = styles->GetFirstElement();
	styles->RemoveAll();

	JFont black = f;
	black.SetColor(colormap->GetBlackColor());

	JFont blue = f;
	blue.SetColor(colormap->GetBlueColor());

	JFont red = f;
	red.SetColor(colormap->GetRedColor());

	const JIndex length = script.GetLength();
	styles->AppendElements(black, length);

	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter c = script.GetCharacter(i);
		if (c == '\\')
			{
			i++;
			}
		else if (c == '$' && i == length)
			{
			styles->SetNextElements(i, 1, red);
			}
		else if (c == '$' && script.GetCharacter(i+1) == '(')
			{
			JIndexRange range;
			const JBoolean ok = CBMacroSubstitute::GetExecRange(script, i+1, &range);
			styles->SetNextElements(i, range.GetLength()+1, ok ? blue : red);
			i = range.last;
			}
		}
}

/******************************************************************************
 AddMacro

 ******************************************************************************/

void
CBMacroManager::AddMacro
	(
	const JCharacter* macro,
	const JCharacter* script
	)
{
	assert( !JStringEmpty(macro) && script != NULL );

	MacroInfo info(new JString(macro), new JString(script));
	assert( info.macro != NULL && info.script != NULL );
	itsMacroList->InsertSorted(info);
}

/******************************************************************************
 CompareMacros (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
CBMacroManager::CompareMacros
	(
	const MacroInfo& m1,
	const MacroInfo& m2
	)
{
	return JCompareStringsCaseInsensitive(m1.macro, m2.macro);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBMacroManager::ReadSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	itsMacroList->DeleteAll();

	JSize count;
	input >> count;

	for (JIndex i=1; i<=count; i++)
		{
		MacroInfo info(new JString, new JString);
		assert( info.macro != NULL && info.script != NULL );
		input >> *(info.macro) >> *(info.script);
		itsMacroList->AppendElement(info);
		}

	if (vers == 0)
		{
		JString geom;
		JCoordinate width;
		input >> geom >> width;
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBMacroManager::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	const JSize count = itsMacroList->GetElementCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
		{
		const MacroInfo info = itsMacroList->GetElement(i);
		output << ' ' << *(info.macro) << ' ' << *(info.script);
		}

	output << ' ';
}

/******************************************************************************
 CBMacroList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
CBMacroList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBMacroManager::MacroInfo info = GetElement(i);
		delete info.macro;
		delete info.script;
		}
	RemoveAll();
}
