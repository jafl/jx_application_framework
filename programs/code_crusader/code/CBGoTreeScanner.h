/******************************************************************************
 CBGoTreeScanner.h

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBGoTreeScanner
#define _H_CBGoTreeScanner

#ifndef _H_CBGoTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer CBGoTreeFlexLexer
#include <FlexLexer.h>
#endif

#include <JPtrArray.h>
#include <JStringPtrMap.h>
#include <JFAID.h>

class CBTree;
class CBClass;

class CBGoTreeScanner : public CBGoTreeFlexLexer
{
public:

	CBGoTreeScanner();

	virtual ~CBGoTreeScanner();

	// written by flex

	JBoolean	CreateClasses(const JString& fileName, const JFAID_t fileID,
							  CBTree* tree, JPtrArray<CBClass>* classList);

private:

	void	ParseTypeContent(CBClass* goClass, const JString& typeContent,
							 const JStringPtrMap<JString>& importMap);

	// not allowed

	CBGoTreeScanner(const CBGoTreeScanner& source);
	CBGoTreeScanner& operator=(const CBGoTreeScanner& source);
};

#endif
