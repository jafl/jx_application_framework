/******************************************************************************
 JXComposeScanner.h

	Interface for the JXComposeScanner class.

	Copyright © 1999 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JXComposeScanner
#define _H_JXComposeScanner

#ifndef _H_JXComposeScannerL
#undef yyFlexLexer
#define yyFlexLexer JXComposeFlexLexer
#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <FlexLexer.h>
#endif

#include <jTypes.h>

class JXComposeRuleList;

class JXComposeScanner : public JXComposeFlexLexer
{
public:

	JXComposeScanner();

	virtual ~JXComposeScanner();

	// written by flex

	void	BuildRuleList(istream& input, JXComposeRuleList* ruleList,
						  const JSize charCount, JBoolean* isCharInWord,
						  JCharacter* diacriticalMap, JIndex* diacriticalMarkType);

private:

	void	CheckIsCharInWord(const JCharacter* name, const JUInt c,
							  const JSize charCount, JBoolean* isCharInWord,
							  JCharacter* diacriticalMap,
							  JIndex* diacriticalMarkType) const;

	// not allowed

	JXComposeScanner(const JXComposeScanner& source);
	const JXComposeScanner& operator=(const JXComposeScanner& source);
};

#endif
