/******************************************************************************
 CBFileNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBFileNode
#define _H_CBFileNode

#include "CBFileNodeBase.h"

class CBFileNode : public CBFileNodeBase
{
public:

	CBFileNode(CBProjectTree* tree, const JString& fileName);
	CBFileNode(std::istream& input, const JFileVersion vers,
			   CBProjectNode* parent);

	virtual ~CBFileNode();

	virtual void	OpenFile() const override;
	virtual void	OpenComplementFile() const override;
	virtual void	ViewPlainDiffs(const bool silent) const override;
	virtual void	ViewVCSDiffs(const bool silent) const override;

	virtual bool	ParseFiles(CBFileListTable* parser,
								   const JPtrArray<JString>& allSuffixList,
								   CBSymbolList* symbolList,
								   CBCTree* cTree, CBDTree* dTree, CBGoTree* goTree,
								   CBJavaTree* javaTree, CBPHPTree* phpTree,
								   JProgressDisplay& pg) const override;

	virtual void	CreateFilesForTemplate(std::istream& input,
										   const JFileVersion vers) const override;
	virtual void	SaveFilesInTemplate(std::ostream& output) const override;

private:

	bool	ParseFile(const JString& fullName, CBFileListTable* parser,
						  const JPtrArray<JString>& allSuffixList,
						  CBSymbolList* symbolList,
						  CBCTree* cTree, CBDTree* dTree, CBGoTree* goTree,
						  CBJavaTree* javaTree, CBPHPTree* phpTree,
						  JProgressDisplay& pg) const;

	// not allowed

	CBFileNode(const CBFileNode& source);
	CBFileNode& operator=(const CBFileNode& source);
};

#endif
