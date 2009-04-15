/******************************************************************************
 JXComposeRuleList.cpp

	Stores X11 Compose rule list.

	BASE CLASS = none

	Copyright © 1999 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JXStdInc.h>
#include <JXComposeRuleList.h>
#include <JXComposeScanner.h>
#include <X11/Xlib.h>
#include <JString.h>
#include <JMinMax.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kComposeHelpPrefixID = "ComposeHelpPrefix::JXComposeRuleList";

/******************************************************************************
 Constructor

 *****************************************************************************/

JXComposeRuleList::JXComposeRuleList
	(
	istream&	input,
	const JSize	charCount,
	JBoolean*	isCharInWord,
	JCharacter*	diacriticalMap,
	JIndex*		diacriticalMarkType
	)
{
	itsRuleList = new JArray<Rule>(100);
	assert( itsRuleList != NULL );
	itsRuleList->SetCompareFunction(CompareRules);

	itsInitialKeySymList = new JArray<KeySym>(10);
	assert( itsInitialKeySymList != NULL );

	itsInputSeq = new JArray<KeySym>;
	assert( itsInputSeq != NULL );

	ClearState();

	JXComposeScanner scanner;
	scanner.BuildRuleList(input, this, charCount, isCharInWord,
						  diacriticalMap, diacriticalMarkType);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXComposeRuleList::~JXComposeRuleList()
{
	const JSize ruleCount = itsRuleList->GetElementCount();
	for (JIndex i=1; i<=ruleCount; i++)
		{
		Rule r = itsRuleList->GetElement(i);
		delete r.inputSeq;
		}
	delete itsRuleList;

	delete itsInitialKeySymList;
	delete itsInputSeq;
}

/******************************************************************************
 AddRule

 *****************************************************************************/

void
JXComposeRuleList::AddRule
	(
	const JArray<KeySym>&	inputSeq,
	const KeySym			outputKeySym
	)
{
	assert( !inputSeq.IsEmpty() );

	if (!IsInitialKeySym(inputSeq.GetFirstElement()))
		{
		itsInitialKeySymList->AppendElement(inputSeq.GetFirstElement());
		}

	Rule r(new JArray<KeySym>(inputSeq), outputKeySym);
	assert( r.inputSeq != NULL );
	itsRuleList->InsertSorted(r);
}

/******************************************************************************
 HandleKeyPress

	Returns kJTrue if the KeyPress event should be processed normally.

 *****************************************************************************/

JBoolean
JXComposeRuleList::HandleKeyPress
	(
	const KeySym	keySym,
	JCharacter*		buffer
	)
{
	if (keySym == XK_Shift_L || keySym == XK_Shift_R)
		{
		return kJTrue;
		}
	else if (!itsIsCollectingFlag && IsInitialKeySym(keySym))
		{
		itsIsCollectingFlag = kJTrue;
		itsInputSeq->RemoveAll();
		itsInputSeq->AppendElement(keySym);
		return kJFalse;
		}
	else if (!itsIsCollectingFlag)
		{
		return kJTrue;
		}
	else
		{
		return MatchesRule(keySym, buffer);
		}
}

/******************************************************************************
 MatchesRule (private)

 *****************************************************************************/

JBoolean
JXComposeRuleList::MatchesRule
	(
	const KeySym	keySym,
	JCharacter*		buffer
	)
{
	itsInputSeq->AppendElement(keySym);

	Rule target(itsInputSeq, XK_space);
	JBoolean found;
	const JIndex index =
		itsRuleList->SearchSorted1(target, JOrderedSetT::kAnyMatch, &found);
	if (found)
		{
		const Rule r = itsRuleList->GetElement(index);
		buffer[0]    = r.GetOutputCharacter();
		ClearState();
		return kJTrue;			// process it (rule fired)
		}
	else if (itsRuleList->IndexValid(index))
		{
		const Rule r = itsRuleList->GetElement(index);

		JBoolean isPrefix;
		CompareRulesWithPrefix(target, r, &isPrefix);
		if (isPrefix)
			{
			return kJFalse;		// don't process it yet (expecting rule to fire)
			}
		}

	ClearState();
	return kJTrue;				// process it (not an input sequence)
}

/******************************************************************************
 IsInitialKeySym (private)

 *****************************************************************************/

JBoolean
JXComposeRuleList::IsInitialKeySym
	(
	const KeySym keySym
	)
	const
{
	const JSize count = itsInitialKeySymList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (keySym == itsInitialKeySymList->GetElement(i))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 CompareRules (static private)

	Sorts by input sequence.

 ******************************************************************************/

JOrderedSetT::CompareResult
JXComposeRuleList::CompareRules
	(
	const Rule& r1,
	const Rule& r2
	)
{
	JBoolean prefix;
	return CompareRulesWithPrefix(r1, r2, &prefix);
}

JOrderedSetT::CompareResult
JXComposeRuleList::CompareRulesWithPrefix
	(
	const Rule&	r1,
	const Rule&	r2,
	JBoolean*	r1IsPrefixOfr2
	)
{
	*r1IsPrefixOfr2 = kJFalse;

	const JSize c1 = (r1.inputSeq)->GetElementCount();
	const JSize c2 = (r2.inputSeq)->GetElementCount();

	const JSize count = JMin(c1, c2);
	for (JIndex i=1; i<=count; i++)
		{
		const KeySym ks1 = (r1.inputSeq)->GetElement(i);
		const KeySym ks2 = (r2.inputSeq)->GetElement(i);

		if (ks1 < ks2)
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		else if (ks1 > ks2)
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		}

	if (c1 < c2)
		{
		*r1IsPrefixOfr2 = kJTrue;
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1 > c2)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 BuildHelp

	Prints an explanation of all the key sequences in HTML.

 ******************************************************************************/

void
JXComposeRuleList::BuildHelp
	(
	JString* text
	)
	const
{
	JArray<Rule> ruleList(*itsRuleList);
	ruleList.SetCompareFunction(CompareForHelp);
	ruleList.Sort();

	*text = JGetString(kComposeHelpPrefixID);

	const JSize ruleCount = ruleList.GetElementCount();
	for (JIndex i=1; i<=ruleCount; i++)
		{
		const Rule r = ruleList.GetElement(i);

		*text += "<tr><td>";
		text->AppendCharacter(r.GetOutputCharacter());
		*text += "</td><td>";

		const JSize keyCount = (r.inputSeq)->GetElementCount();
		for (JIndex j=1; j<=keyCount; j++)
			{
			*text += XKeysymToString((r.inputSeq)->GetElement(j));
			text->AppendCharacter(' ');
			}

		*text += "</td></tr>";
		}

	*text +=
		"</table>"
		"</body>"
		"</html>";
}

/******************************************************************************
 CompareForHelp (static private)

	Sorts by output character and then by input sequence.

 ******************************************************************************/

JOrderedSetT::CompareResult
JXComposeRuleList::CompareForHelp
	(
	const Rule& r1,
	const Rule& r2
	)
{
	const JCharacter c1 = r1.GetOutputCharacter();
	const JCharacter c2 = r2.GetOutputCharacter();
	if (c1 < c2)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1 > c2)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else
		{
		return CompareRules(r1, r2);
		}
}

#define JTemplateType JXComposeRuleList::Rule
#include <JArray.tmpls>
#undef JTemplateType
