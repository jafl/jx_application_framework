/******************************************************************************
 CBDTree.h

	Interface for the CBDTree Class

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBDTree
#define _H_CBDTree

#include "CBTree.h"

class CBDTreeDirector;
class CBCClass;
class CBClass;

namespace CB::Tree::D { class Scanner; }

class CBDTree : public CBTree
{
public:

	CBDTree(CBDTreeDirector* director, const JSize marginWidth);
	CBDTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			CBDTreeDirector* director, const JSize marginWidth,
			CBDirList* dirList);

	virtual ~CBDTree();

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	CB::Tree::D::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static CBClass* StreamInCClass(std::istream& input, const JFileVersion vers,
								   CBTree* tree);

	// not allowed

	CBDTree(const CBDTree& source);
	CBDTree& operator=(const CBDTree& source);
};

#endif
