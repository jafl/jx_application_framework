/******************************************************************************
 CBJavaTreeScanner.h

	Copyright © 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBJavaTreeScanner
#define _H_CBJavaTreeScanner

#ifndef _H_CBJavaTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer CBJavaTreeFlexLexer
#include <FlexLexer.h>
#endif

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFAID.h>

class CBTree;
class CBClass;

class CBJavaTreeScanner : public CBJavaTreeFlexLexer
{
public:

	CBJavaTreeScanner();

	virtual ~CBJavaTreeScanner();

	// written by flex

	JBoolean	CreateClass(const JCharacter* fileName, const JFAID_t fileID,
							CBTree* tree, CBClass** theClass);

private:

	// not allowed

	CBJavaTreeScanner(const CBJavaTreeScanner& source);
	CBJavaTreeScanner& operator=(const CBJavaTreeScanner& source);
};

#endif
