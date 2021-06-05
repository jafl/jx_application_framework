/******************************************************************************
 CBGoTreeScanner.cpp

	Extracts class information from Go source file.

	BASE CLASS = CBGoTreeFlexLexer

	Copyright (C) 2021 John Lindal.

 ******************************************************************************/

#include "CBGoTreeScanner.h"
#include "CBClass.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBGoTreeScanner::CBGoTreeScanner()
	:
	CBGoTreeFlexLexer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBGoTreeScanner::~CBGoTreeScanner()
{
}

/******************************************************************************
 ParseTypeContent (private)

 ******************************************************************************/

static const JRegex separator(";|\n");
static const JRegex nsid("^\\*?(([_[:alpha:]][_[:alnum:]]*\\.)?[_[:alpha:]][_[:alnum:]]*)$");

void
CBGoTreeScanner::ParseTypeContent
	(
	CBClass*						goClass,
	const JString&					typeContent,
	const JStringPtrMap<JString>&	importMap
	)
{
	JPtrArray<JString> lineList(JPtrArrayT::kDeleteAll);
	typeContent.Split(separator, &lineList);

	for (JString* line : lineList)
		{
		line->TrimWhitespace();

		const JStringMatch m = nsid.Match(*line, kJTrue);
		if (m.IsEmpty())
			{
			continue;
			}
		JString parent = m.GetSubstring(1);

		const JString* package;
		if (!m.GetUtf8ByteRange(2).IsEmpty() &&
			importMap.GetElement(m.GetSubstring(2), &package))
			{
			JStringIterator iter(&parent);
			iter.Next(".");
			iter.RemoveAllPrev();
			parent.Prepend(".");
			parent.Prepend(*package);
			}

		goClass->AddParent(CBClass::kInheritPublic, parent);
		}
}
