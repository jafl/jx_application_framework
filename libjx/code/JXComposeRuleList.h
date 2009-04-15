/******************************************************************************
 JXComposeRuleList.h

	Interface for the JXComposeRuleList class.

	Copyright © 1999 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JXComposeRuleList
#define _H_JXComposeRuleList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>
#include <jXKeysym.h>
#include <X11/X.h>

class JString;

class JXComposeRuleList
{
public:

	JXComposeRuleList(istream& input, const JSize charCount,
					  JBoolean* isCharInWord, JCharacter* diacriticalMap,
					  JIndex* diacriticalMarkType);

	virtual ~JXComposeRuleList();

	void		AddRule(const JArray<KeySym>& inputSeq, const KeySym outputKeySym);
	JBoolean	HandleKeyPress(const KeySym keySym, JCharacter* buffer);
	void		ClearState();

	void	BuildHelp(JString* text) const;

private:

	struct Rule
	{
		JArray<KeySym>*	inputSeq;
		KeySym			outputKeySym;

		Rule()
			:
			inputSeq(NULL), outputKeySym(XK_space)
		{ };

		Rule(JArray<KeySym>* i, const KeySym o)
			:
			inputSeq(i), outputKeySym(o)
		{ };

		JCharacter
		GetOutputCharacter() const
		{
			return static_cast<JCharacter>(outputKeySym & 0xFF);
		};
	};

private:

	JArray<Rule>*	itsRuleList;
	JArray<KeySym>*	itsInitialKeySymList;	// list of KeySyms that begin an input sequence (optimization)

	JBoolean		itsIsCollectingFlag;
	JArray<KeySym>*	itsInputSeq;			// list of KeySyms that have been accumulated

private:

	JBoolean	MatchesRule(const KeySym keySym, JCharacter* buffer);
	JBoolean	IsInitialKeySym(const KeySym keySym) const;

	static JOrderedSetT::CompareResult
		CompareRules(const Rule& r1, const Rule& r2);
	static JOrderedSetT::CompareResult
		CompareRulesWithPrefix(const Rule& r1, const Rule& r2, JBoolean* r1IsPrefixOfr2);
	static JOrderedSetT::CompareResult
		CompareForHelp(const Rule& r1, const Rule& r2);

	// not allowed

	JXComposeRuleList(const JXComposeRuleList& source);
	const JXComposeRuleList& operator=(const JXComposeRuleList& source);
};


/******************************************************************************
 ClearState

 *****************************************************************************/

inline void
JXComposeRuleList::ClearState()
{
	itsIsCollectingFlag = kJFalse;
}

#endif
