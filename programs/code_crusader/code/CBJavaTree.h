/******************************************************************************
 CBJavaTree.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBJavaTree
#define _H_CBJavaTree

#include "CBTree.h"

class CBJavaTreeDirector;
class CBJavaClass;
class CBClass;

namespace CB::Tree::Java { class Scanner; }

class CBJavaTree : public CBTree
{
public:

	CBJavaTree(CBJavaTreeDirector* director, const JSize marginWidth);
	CBJavaTree(std::istream& projInput, const JFileVersion projVers,
			   std::istream* setInput, const JFileVersion setVers,
			   std::istream* symInput, const JFileVersion symVers,
			   CBJavaTreeDirector* director, const JSize marginWidth,
			   CBDirList* dirList);

	virtual ~CBJavaTree();

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	CB::Tree::Java::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static CBClass* StreamInJavaClass(std::istream& input, const JFileVersion vers,
									  CBTree* tree);

	// not allowed

	CBJavaTree(const CBJavaTree& source);
	CBJavaTree& operator=(const CBJavaTree& source);
};

#endif
