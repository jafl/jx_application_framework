/******************************************************************************
 CBCPreprocessor.cpp

	Implements a trivial C preprocessor that only understands simple
	identifier replacement.

	BASE CLASS = none

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBCPreprocessor.h"
#include <JPtrArray-JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCPreprocessor::CBCPreprocessor()
{
	itsMacroList = jnew CBPPMacroList;
	assert( itsMacroList != nullptr );
	itsMacroList->SetSortOrder(JListT::kSortAscending);
	itsMacroList->SetCompareFunction(CompareMacros);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCPreprocessor::~CBCPreprocessor()
{
	itsMacroList->DeleteAll();
	jdelete itsMacroList;
}

/******************************************************************************
 Preprocess

	Returns kJTrue if it changed the text.

 ******************************************************************************/

JBoolean
CBCPreprocessor::Preprocess
	(
	JString* text
	)
	const
{
	JBoolean changed = kJFalse;

	const JSize count = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const MacroInfo info   = itsMacroList->GetElement(i);
		const JSize nameLength = (info.name)->GetLength();

		JIndex j=1;
		while (text->LocateNextSubstring(*(info.name), &j))
			{
			if (IsEntireWord(*text, j, nameLength))
				{
				// The only action we prevent by incrementing j is
				// an infinite loop.

				text->ReplaceSubstring(j, j + nameLength-1, *(info.value));
				changed = kJTrue;
				j += (info.value)->GetLength();
				}
			else
				{
				j++;
				}
			}
		}

	return changed;
}

/******************************************************************************
 IsEntireWord (private)

 ******************************************************************************/

JBoolean
CBCPreprocessor::IsEntireWord
	(
	const JString&	text,
	const JIndex	wordIndex,
	const JSize		wordLength
	)
	const
{
	return JI2B(!(wordIndex > 1 &&
				  IsIDCharacter(text.GetCharacter(wordIndex)) &&
				  IsIDCharacter(text.GetCharacter(wordIndex - 1))) &&
				!(wordIndex + wordLength <= text.GetLength() &&
				  IsIDCharacter(text.GetCharacter(wordIndex + wordLength - 1)) &&
				  IsIDCharacter(text.GetCharacter(wordIndex + wordLength))));
}

/******************************************************************************
 IsIDCharacter (private)

 ******************************************************************************/

inline int
CBCPreprocessor::IsIDCharacter
	(
	const JUtf8Character& c
	)
	const
{
	return (c.IsAlnum() || c == '_');
}

/******************************************************************************
 PrintMacrosForCTags

	Returns kJTrue if any macros have been defined.

	Format:  macro/macro+/macro=value

 ******************************************************************************/

static const JUtf8Byte* kDefineMacroOption = "-I ";	// macro/macro+/macro=value

void
CBCPreprocessor::PrintMacrosForCTags
	(
	std::ostream& output
	)
	const
{
	const JSize count = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const MacroInfo info = itsMacroList->GetElement(i);

		if (!info.name->Contains(" ") &&
			!info.name->Contains("\t") &&
			!info.value->Contains(" ") &&
			!info.value->Contains("\t"))
			{
			output << kDefineMacroOption;
			info.name->Print(output);

			if (!info.value->IsEmpty())
				{
				output << '=';
				info.value->Print(output);
				}

			output << std::endl;
			}
		}
}

/******************************************************************************
 DefineMacro

 ******************************************************************************/

void
CBCPreprocessor::DefineMacro
	(
	const JString& name,
	const JString& value
	)
{
	assert( !name.IsEmpty() );

	MacroInfo info(jnew JString(name), jnew JString(value));
	assert( info.name != nullptr && info.value != nullptr );
	itsMacroList->InsertSorted(info);
}

/******************************************************************************
 CompareMacros (static private)

 ******************************************************************************/

JListT::CompareResult
CBCPreprocessor::CompareMacros
	(
	const MacroInfo& m1,
	const MacroInfo& m2
	)
{
	return JCompareStringsCaseInsensitive(m1.name, m2.name);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBCPreprocessor::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	assert( vers >= 28 );

	itsMacroList->DeleteAll();

	if (vers >= 71)
		{
		input >> std::ws;
		JIgnoreLine(input);
		}

	if (vers <= 71)
		{
		JSize count;
		input >> count;

		for (JIndex i=1; i<=count; i++)
			{
			ReadMacro(input, vers);
			}
		}
	else
		{
		while (1)
			{
			JBoolean keepGoing;
			input >> JBoolFromString(keepGoing);
			if (!keepGoing)
				{
				break;
				}

			ReadMacro(input, vers);
			}
		}
}

/******************************************************************************
 ReadMacro (private)

 ******************************************************************************/

void
CBCPreprocessor::ReadMacro
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	MacroInfo info(jnew JString, jnew JString);
	assert( info.name != nullptr && info.value != nullptr );
	input >> *info.name >> *info.value;
	itsMacroList->AppendElement(info);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBCPreprocessor::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << "# C preprocessor\n";

	const JSize count = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		output << JBoolToString(kJTrue);

		const MacroInfo info = itsMacroList->GetElement(i);
		output << ' ' << *(info.name);
		output << ' ' << *(info.value);
		output << '\n';
		}

	output << JBoolToString(kJFalse) << '\n';
}

/******************************************************************************
 CBPPMacroList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
CBPPMacroList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBCPreprocessor::MacroInfo info = GetElement(i);
		jdelete info.name;
		jdelete info.value;
		}

	RemoveAll();
}
