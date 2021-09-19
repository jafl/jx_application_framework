/******************************************************************************
 CBCTree.h

	Interface for the CBCTree Class

	Copyright © 1995-99 John Lindal.

 ******************************************************************************/

#ifndef _H_CBCTree
#define _H_CBCTree

#include "CBTree.h"
#include "CBCtagsUser.h"

class CBCTreeDirector;
class CBCPreprocessor;
class CBCClass;
class CBClass;

namespace CB::Tree::C { class Scanner; }

class CBCTree : public CBTree, public CBCtagsUser
{
public:

	CBCTree(CBCTreeDirector* director, const JSize marginWidth);
	CBCTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			CBCTreeDirector* director, const JSize marginWidth,
			CBDirList* dirList);

	virtual ~CBCTree();

	CBCPreprocessor*	GetCPreprocessor() const;

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBCPreprocessor*	itsCPP;
	CB::Tree::C::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	void	CBCTreeX();

	static CBClass* StreamInCClass(std::istream& input, const JFileVersion vers,
								   CBTree* tree);

	// not allowed

	CBCTree(const CBCTree& source);
	CBCTree& operator=(const CBCTree& source);
};


/******************************************************************************
 GetCPreprocessor

 ******************************************************************************/

inline CBCPreprocessor*
CBCTree::GetCPreprocessor()
	const
{
	return itsCPP;
}

#endif
