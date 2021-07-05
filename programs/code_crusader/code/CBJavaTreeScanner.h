/******************************************************************************
 CBJavaTreeScanner.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBJavaTreeScanner
#define _H_CBJavaTreeScanner

#ifndef _H_CBJavaTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer CBJavaTreeFlexLexer
#include <FlexLexer.h>
#endif

#include <JPtrArray.h>
#include <JFAID.h>

class CBTree;
class CBClass;

class CBJavaTreeScanner : public CBJavaTreeFlexLexer
{
public:

	CBJavaTreeScanner();

	virtual ~CBJavaTreeScanner();

	// written by flex

	JBoolean	CreateClasses(const JString& fileName, const JFAID_t fileID,
							  CBTree* tree, JPtrArray<CBClass>* classList);

private:

	JBoolean	itsAbstractFlag;
	JBoolean	itsPublicFlag;
	JBoolean	itsFinalFlag;
	CBClass*	itsCurrentClass;

private:

	void	ResetState();

	// not allowed

	CBJavaTreeScanner(const CBJavaTreeScanner& source);
	CBJavaTreeScanner& operator=(const CBJavaTreeScanner& source);
};

#endif
