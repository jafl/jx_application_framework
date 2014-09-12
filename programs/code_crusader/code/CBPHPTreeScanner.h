/******************************************************************************
 CBPHPTreeScanner.h

	Copyright © 2014 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPHPTreeScanner
#define _H_CBPHPTreeScanner

#ifndef _H_CBPHPTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPHPTreeFlexLexer
#include <FlexLexer.h>
#endif

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFAID.h>

class CBTree;
class CBClass;

class CBPHPTreeScanner : public CBPHPTreeFlexLexer
{
public:

	CBPHPTreeScanner();

	virtual ~CBPHPTreeScanner();

	// written by flex

	JBoolean	CreateClass(const JCharacter* fileName, const JFAID_t fileID,
							CBTree* tree, CBClass** theClass);

private:

	// not allowed

	CBPHPTreeScanner(const CBPHPTreeScanner& source);
	CBPHPTreeScanner& operator=(const CBPHPTreeScanner& source);
};

#endif
