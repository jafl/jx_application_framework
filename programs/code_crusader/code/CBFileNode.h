/******************************************************************************
 CBFileNode.h

	Copyright (C) 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFileNode
#define _H_CBFileNode

#include "CBFileNodeBase.h"

class CBFileNode : public CBFileNodeBase
{
public:

	CBFileNode(CBProjectTree* tree, const JCharacter* fileName);
	CBFileNode(istream& input, const JFileVersion vers,
			   CBProjectNode* parent);

	virtual ~CBFileNode();

	virtual void	OpenFile() const;
	virtual void	OpenComplementFile() const;
	virtual void	ViewPlainDiffs(const JBoolean silent) const;
	virtual void	ViewVCSDiffs(const JBoolean silent) const;

	virtual JBoolean	ParseFiles(CBFileListTable* parser,
								   const JPtrArray<JString>& allSuffixList,
								   CBSymbolList* symbolList,
								   CBCTree* cTree, CBJavaTree* javaTree,
								   CBPHPTree* phpTree,
								   JProgressDisplay& pg) const;

	virtual void	CreateFilesForTemplate(istream& input,
										   const JFileVersion vers) const;
	virtual void	SaveFilesInTemplate(ostream& output) const;

private:

	JBoolean	ParseFile(const JString& fullName, CBFileListTable* parser,
						  const JPtrArray<JString>& allSuffixList,
						  CBSymbolList* symbolList,
						  CBCTree* cTree, CBJavaTree* javaTree, CBPHPTree* phpTree,
						  JProgressDisplay& pg) const;

	// not allowed

	CBFileNode(const CBFileNode& source);
	CBFileNode& operator=(const CBFileNode& source);
};

#endif
