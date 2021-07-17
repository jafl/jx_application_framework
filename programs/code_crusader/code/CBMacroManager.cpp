/******************************************************************************
 CBMacroManager.cpp

	Stores a list of macro strings and associated actions.  The action is
	performed after the string is typed followed by a tab.

	Macros are specified by plain text, not regular expressions, because
	each macro should have a unique name.  Anything more complicated should
	be done with regex search & replace.

	BASE CLASS = none

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBMacroManager.h"
#include "CBMacroSubstitute.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbmUtil.h"
#include <JStringIterator.h>
#include <JColorManager.h>
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
	itsMacroList = jnew CBMacroList;
	assert( itsMacroList != nullptr );
	itsMacroList->SetSortOrder(JListT::kSortAscending);
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

		MacroInfo newInfo(jnew JString(*(oldInfo.macro)),
						  jnew JString(*(oldInfo.script)));
		assert( newInfo.macro != nullptr && newInfo.script != nullptr );
		itsMacroList->AppendElement(newInfo);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBMacroManager::~CBMacroManager()
{
	itsMacroList->DeleteAll();
	jdelete itsMacroList;
}

/******************************************************************************
 Perform

	Returns true if it found a script and performed it.

 ******************************************************************************/

bool
CBMacroManager::Perform
	(
	const JStyledText::TextIndex&	caretIndex,
	CBTextDocument*					doc
	)
{
	if (caretIndex.charIndex <= 1)
		{
		return false;
		}

	const JStyledText* st = doc->GetTextEditor()->GetText();
	const JUtf8Byte* text = st->GetText().GetBytes();

	const JSize macroCount = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=macroCount; i++)
		{
		const MacroInfo info = itsMacroList->GetElement(i);
		if (info.macro->GetCharacterCount() <= caretIndex.charIndex-1)
			{
			const JStyledText::TextIndex j =
				st->AdjustTextIndex(caretIndex, -(JInteger)info.macro->GetCharacterCount());

			if (info.macro->GetByteCount() == caretIndex.byteIndex - j.byteIndex &&
				JString::CompareMaxNBytes(text + j.byteIndex-1, info.macro->GetBytes(),
										  info.macro->GetByteCount()) == 0)
				{
				JStringIterator iter(st->GetText());
				iter.UnsafeMoveTo(kJIteratorStartBefore, j.charIndex, j.byteIndex);

				JUtf8Character c;
				if (iter.AtBeginning() ||
					(iter.Next(&c, kJIteratorStay) && !CBMIsCharacterInWord(c)) ||
					(iter.Prev(&c, kJIteratorStay) && !CBMIsCharacterInWord(c)))
					{
					iter.Invalidate();
					Perform(*info.script, doc);
					return true;
					}
				}
			}
		}

	return false;
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
	te->GetText()->DeactivateCurrentUndo();

	bool onDisk;
	const JString fullName = JPrepArgForExec(doc->GetFullName(&onDisk));
	const JString path     = JPrepArgForExec(doc->GetFilePath());
	const JString fileName = JPrepArgForExec(doc->GetFileName());

	JString root, suffix;
	if (JSplitRootAndSuffix(doc->GetFileName(), &root, &suffix))
		{
		root = JPrepArgForExec(root);
		suffix.Prepend(".");
		}

	const JIndex charIndex = te->GetInsertionCharIndex();
	const JIndex lineIndex = te->GetLineForChar(charIndex);
	const JIndex lineStart = te->GetLineCharStart(lineIndex);

	const JString lineIndexStr((JUInt64) lineIndex);

	JString lineStr;
	if (charIndex > lineStart)
		{
		JStringIterator iter(te->GetText()->GetText(), kJIteratorStartBefore, lineStart);
		iter.BeginMatch();
		iter.MoveTo(kJIteratorStartAfter, charIndex);
		lineStr = JPrepArgForExec(iter.FinishMatch().GetString());
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

	JStringIterator iter(s);
	JUtf8Character c;
	while (iter.Next(&c))
		{
		te->JXTEBase::HandleKeyPress(c, 0, modifiers);
		}
}

/******************************************************************************
 HighlightErrors (static)

 ******************************************************************************/

void
CBMacroManager::HighlightErrors
	(
	const JString&		script,
	JRunArray<JFont>*	styles
	)
{
	const JFont f = styles->GetFirstElement();
	styles->RemoveAll();

	JFont black = f;
	black.SetColor(JColorManager::GetBlackColor());

	JFont blue = f;
	blue.SetColor(JColorManager::GetBlueColor());

	JFont red = f;
	red.SetColor(JColorManager::GetRedColor());

	styles->AppendElements(black, script.GetCharacterCount());

	JStringIterator siter(script);
	JRunArrayIterator<JFont> fiter(styles);
	JUtf8Character c;
	while (siter.Next(&c))
		{
		if (c == '\\')
			{
			siter.SkipNext();
			fiter.SkipNext(2);
			}
		else if (c == '$' && siter.AtEnd())
			{
			fiter.SetPrev(red, kJIteratorStay);
			}
		else if (c == '$' && siter.Next(&c, kJIteratorStay) && c == '(')
			{
			siter.SkipNext();
			const bool ok = CBMBalanceForward(kCBCLang, &siter, &c);
			fiter.SetNext(ok ? blue : red, siter.GetPrevCharacterIndex() - fiter.GetNextElementIndex() + 1);
			}
		else
			{
			fiter.SkipNext();
			}
		}
}

/******************************************************************************
 AddMacro

 ******************************************************************************/

void
CBMacroManager::AddMacro
	(
	const JUtf8Byte* macro,
	const JUtf8Byte* script
	)
{
	assert( !JString::IsEmpty(macro) && script != nullptr );

	MacroInfo info(jnew JString(macro), jnew JString(script));
	assert( info.macro != nullptr && info.script != nullptr );
	itsMacroList->InsertSorted(info);
}

/******************************************************************************
 CompareMacros (static private)

 ******************************************************************************/

JListT::CompareResult
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
	std::istream& input
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
		MacroInfo info(jnew JString, jnew JString);
		assert( info.macro != nullptr && info.script != nullptr );
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
	std::ostream& output
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
		jdelete info.macro;
		jdelete info.script;
		}
	RemoveAll();
}
