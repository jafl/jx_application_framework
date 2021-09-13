/******************************************************************************
 GDBGetLocalVars.cpp

	BASE CLASS = CMGetLocalVars

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetLocalVars.h"
#include "CMVarNode.h"
#include "GDBVarTreeParser.h"
#include "cmGlobals.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetLocalVars::GDBGetLocalVars
	(
	CMVarNode* rootNode
	)
	:
	CMGetLocalVars(JString(
		"set print pretty off\n"
		"set print array off\n"
		"set print repeats 0\n"
		"set width 0\n"
		"info args\n"
		"echo -----\\n\n"
		"info locals", JString::kNoCopy)),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetLocalVars::~GDBGetLocalVars()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JString kSeparator("\n-----\n", JString::kNoCopy);
static const JRegex varPattern     = "^([^=]+)=(.*)$";
static const JRegex gdb7RefPattern = "^@0[xX][[:xdigit:]]+$";

void
GDBGetLocalVars::HandleSuccess
	(
	const JString& data
	)
{
	if (!data.Contains(kSeparator))
		{
		itsRootNode->DeleteAllChildren();
		return;
		}

	JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
	data.Split(kSeparator, &s, 2);

	JString *argData = s.GetElement(1),
			*varData = s.GetElement(2);

	// build list of arguments

	JPtrArray<JString> nameList(JPtrArrayT::kDeleteAll),
					   valueList(JPtrArrayT::kDeleteAll);

	CleanVarString(argData);

	JStringIterator argIter(*argData);
	while (argIter.Next(varPattern))
		{
		const JStringMatch& m = argIter.GetLastMatch();

		auto* name = jnew JString(m.GetSubstring(1));
		assert( name != nullptr );
		name->TrimWhitespace();
		nameList.Append(name);

		auto* value = jnew JString(m.GetSubstring(2));
		assert( value != nullptr );
		value->TrimWhitespace();
		valueList.Append(value);
		}
	argIter.Invalidate();

	// build list of local variables in reverse order

	CleanVarString(varData);

	const JIndex insertionIndex = nameList.GetElementCount()+1;

	JStringIterator varIter(*varData);
	while (varIter.Next(varPattern))
		{
		const JStringMatch& m = varIter.GetLastMatch();

		auto* name = jnew JString(m.GetSubstring(1));
		assert( name != nullptr );
		name->TrimWhitespace();
		nameList.InsertAtIndex(insertionIndex, name);

		auto* value = jnew JString(m.GetSubstring(2));
		assert( value != nullptr );
		value->TrimWhitespace();
		valueList.InsertAtIndex(insertionIndex, value);
		}
	varIter.Invalidate();

	// delete existing nodes beyond the first one that doesn't match the
	// new variable names

	const JSize newCount = nameList.GetElementCount();
	JSize origCount      = itsRootNode->GetChildCount();

	while (origCount > newCount)
		{
		jdelete itsRootNode->GetChild(origCount);
		origCount--;
		}

	// origCount <= newCount

	for (JIndex i=1; i<=origCount; i++)
		{
		if ((itsRootNode->GetVarChild(i))->GetName() !=
			*(nameList.GetElement(i)))
			{
			if (i == 1)		// optimize since likely to be most common case
				{
				itsRootNode->DeleteAllChildren();
				origCount = 0;
				}
			else
				{
				while (origCount >= i)
					{
					jdelete itsRootNode->GetChild(origCount);
					origCount--;
					}
				}
			break;	// for clarity:  would happen anyway
			}
		}

	// update/create nodes

	for (JIndex i=1; i<=newCount; i++)
		{
		CMVarNode* node = nullptr;
		if (i <= origCount)
			{
			node = itsRootNode->GetVarChild(i);
			}
		else
			{
			node = CMGetLink()->CreateVarNode(nullptr, *(nameList.GetElement(i)), JString::empty, JString::empty);
			assert( node != nullptr );
			itsRootNode->Append(node);	// avoid automatic update
			}

		const JString* value = valueList.GetElement(i);
		if (gdb7RefPattern.Match(*value))
			{
			node->UpdateValue();
			}
		else
			{
			GDBVarTreeParser parser;
			if (parser.Parse(*value) == 0)
				{
				parser.ReportRecoverableError();
				node->UpdateValue(parser.GetRootNode());
				}
			else
				{
				node->UpdateFailed(*(valueList.GetElement(i)));
				}
			}
		}
}

/******************************************************************************
 CleanVarString (private)

	Clean up gdb output that would otherwise confuse the parser.

	std::list contains ,\n between items.  Newline is unnecessary.

 ******************************************************************************/

static const JRegex parseError1Pattern = "[{,]\n";
static const JRegex parseError2Pattern = "\n\\}";

void
GDBGetLocalVars::CleanVarString
	(
	JString* s
	)
{
	JStringIterator iter(s);
	while (iter.Next(parseError1Pattern))
		{
		iter.RemovePrev();
		}

	while (iter.Next(parseError2Pattern))
		{
		iter.SkipPrev(2);
		iter.RemovePrev();
		}
}
