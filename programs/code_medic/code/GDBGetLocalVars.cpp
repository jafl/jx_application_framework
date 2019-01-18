/******************************************************************************
 GDBGetLocalVars.cpp

	BASE CLASS = CMGetLocalVars

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetLocalVars.h"
#include "CMVarNode.h"
#include "GDBVarTreeParser.h"
#include "cmGlobals.h"
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
	CMGetLocalVars(	"set print pretty off\n"
					"set print array off\n"
					"set print repeats 0\n"
					"set width 0\n"
					"info args\n"
					"echo -----\\n\n"
					"info locals"),
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

static const JCharacter* kSeparator = "\n-----\n";
static const JRegex varPattern      = "^([^=]+)=(.*)$";
static const JRegex gdb7RefPattern  = "^@0[xX][[:xdigit:]]+$";

void
GDBGetLocalVars::HandleSuccess
	(
	const JString& data
	)
{
	JIndex separatorIndex;
	if (!data.LocateSubstring(kSeparator, &separatorIndex))
		{
		itsRootNode->DeleteAllChildren();
		return;
		}

	JString argData, varData;
	if (separatorIndex > 1)
		{
		argData = data.GetSubstring(1, separatorIndex-1);
		}
	separatorIndex += strlen(kSeparator);
	if (separatorIndex <= data.GetLength())
		{
		varData = data.GetSubstring(separatorIndex, data.GetLength());
		}

	// build list of arguments

	JPtrArray<JString> nameList(JPtrArrayT::kDeleteAll),
					   valueList(JPtrArrayT::kDeleteAll);

	CleanVarString(&argData);

	JIndexRange matchedRange;
	JArray<JIndexRange> matchList;
	while (varPattern.MatchAfter(argData, matchedRange, &matchList))
		{
		JString* name = jnew JString(argData.GetSubstring(matchList.GetElement(2)));
		assert( name != nullptr );
		name->TrimWhitespace();
		nameList.Append(name);

		JString* value = jnew JString(argData.GetSubstring(matchList.GetElement(3)));
		assert( value != nullptr );
		value->TrimWhitespace();
		valueList.Append(value);

		matchedRange = matchList.GetFirstElement();
		}

	// build list of local variables in reverse order

	CleanVarString(&varData);

	matchedRange.SetToNothing();
	const JIndex insertionIndex = nameList.GetElementCount()+1;
	while (varPattern.MatchAfter(varData, matchedRange, &matchList))
		{
		JString* name = jnew JString(varData.GetSubstring(matchList.GetElement(2)));
		assert( name != nullptr );
		name->TrimWhitespace();
		nameList.InsertAtIndex(insertionIndex, name);

		JString* value = jnew JString(varData.GetSubstring(matchList.GetElement(3)));
		assert( value != nullptr );
		value->TrimWhitespace();
		valueList.InsertAtIndex(insertionIndex, value);

		matchedRange = matchList.GetFirstElement();
		}

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
			node = CMGetLink()->CreateVarNode(nullptr, *(nameList.GetElement(i)), nullptr, "");
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
			GDBVarTreeParser parser(*value);
			if (parser.yyparse() == 0)
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
	JIndexRange r;
	while (parseError1Pattern.MatchAfter(*s, r, &r))
		{
		s->RemoveSubstring(r.last, r.last);
		r.last--;
		}

	r.SetToNothing();
	while (parseError2Pattern.MatchAfter(*s, r, &r))
		{
		s->RemoveSubstring(r.first, r.first);
		r.last--;
		}
}
