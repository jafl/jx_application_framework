/******************************************************************************
 CBPHPTree.h

	Copyright © 2014 John Lindal.

 ******************************************************************************/

#ifndef _H_CBPHPTree
#define _H_CBPHPTree

#include "CBTree.h"

class CBPHPTreeDirector;
class CBPHPClass;
class CBClass;

namespace CB::Tree::PHP { class Scanner; }

class CBPHPTree : public CBTree
{
public:

	CBPHPTree(CBPHPTreeDirector* director, const JSize marginWidth);
	CBPHPTree(std::istream& projInput, const JFileVersion projVers,
			  std::istream* setInput, const JFileVersion setVers,
			  std::istream* symInput, const JFileVersion symVers,
			  CBPHPTreeDirector* director, const JSize marginWidth,
			  CBDirList* dirList);

	virtual ~CBPHPTree();

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	CB::Tree::PHP::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static CBClass* StreamInPHPClass(std::istream& input, const JFileVersion vers,
									 CBTree* tree);

	// not allowed

	CBPHPTree(const CBPHPTree& source);
	CBPHPTree& operator=(const CBPHPTree& source);
};

#endif
