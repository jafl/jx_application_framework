/******************************************************************************
 CBDTreeScanner.h

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBDTreeScanner
#define _H_CBDTreeScanner

#ifndef _H_CBDTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer CBDTreeFlexLexer
#include <FlexLexer.h>
#endif

#include <JPtrArray.h>
#include <JFAID.h>

class CBTree;
class CBClass;

class CBDTreeScanner : public CBDTreeFlexLexer
{
public:

	CBDTreeScanner();

	virtual ~CBDTreeScanner();

	// written by flex

	JBoolean	CreateClasses(const JString& fileName, const JFAID_t fileID,
							  CBTree* tree, JPtrArray<CBClass>* classList);

private:

	JBoolean	itsAbstractFlag;
	JBoolean	itsFinalFlag;
	JBoolean	itsTemplateFlag;
	CBClass*	itsCurrentClass;

private:

	void	ResetState();

	// not allowed

	CBDTreeScanner(const CBDTreeScanner& source);
	CBDTreeScanner& operator=(const CBDTreeScanner& source);
};

#endif
