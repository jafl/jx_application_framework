/******************************************************************************
 CBCTreeScanner.h

	Copyright (C) 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBCTreeScanner
#define _H_CBCTreeScanner

#ifndef _H_CBCTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer CBCTreeFlexLexer
#include <FlexLexer.h>
#endif

#include <JPtrArray.h>
#include <JFAID.h>

class CBTree;
class CBClass;

class CBCTreeScanner : public CBCTreeFlexLexer
{
public:

	CBCTreeScanner();

	virtual ~CBCTreeScanner();

	// written by flex

	JBoolean	CreateClasses(const JString& buffer, const JFAID_t fileID,
							  CBTree* tree, JPtrArray<CBClass>* classList);

private:

	JBoolean	itsTemplateFlag;
	CBClass*	itsCurrentClass;

private:

	void	ResetState();

	// not allowed

	CBCTreeScanner(const CBCTreeScanner& source);
	CBCTreeScanner& operator=(const CBCTreeScanner& source);
};

#endif
