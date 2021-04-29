/******************************************************************************
 CBPHPTreeScanner.h

	Copyright (C) 2014 John Lindal.

 ******************************************************************************/

#ifndef _H_CBPHPTreeScanner
#define _H_CBPHPTreeScanner

#ifndef _H_CBPHPTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPHPTreeFlexLexer
#include <FlexLexer.h>
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

	JBoolean	CreateClass(const JString& fileName, const JFAID_t fileID,
							CBTree* tree, CBClass** theClass);

private:

	// not allowed

	CBPHPTreeScanner(const CBPHPTreeScanner& source);
	CBPHPTreeScanner& operator=(const CBPHPTreeScanner& source);
};

#endif
