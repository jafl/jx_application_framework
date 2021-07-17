/******************************************************************************
 CBTestTree.h

	Interface for the CBTestTree Class

	Copyright (C) 1995-99 John Lindal.

 ******************************************************************************/

#ifndef _H_CBTestTree
#define _H_CBTestTree

#include "CBTree.h"
#include "CBCtagsUser.h"

class CBTestTreeScanner;
class CBTestTreeDirector;
class CBCPreprocessor;
class CBCClass;
class CBClass;

enum FooBar
{
	a, b, c
};

class CBTestTree : public CBTree, public TEST_MACRO
{
public:

	CBTestTree(CBTestTreeDirector* director, const JSize marginWidth);
	CBTestTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			CBTestTreeDirector* director, const JSize marginWidth,
			CBDirList* dirList);

	virtual ~CBTestTree();

	CBCPreprocessor*	GetCPreprocessor() const;

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

protected:

	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;
	virtual void		ParseFile(const JString& fileName, const JFAID_t id) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBCPreprocessor*	itsCPP;
	CBTestTreeScanner*		itsClassNameLexer;	// nullptr unless parsing

private:

	void	CBTestTreeX();

	static CBClass* StreamInCClass(std::istream& input, const JFileVersion vers,
								   CBTree* tree);

	// not allowed

	CBTestTree(const CBTestTree& source);
	CBTestTree& operator=(const CBTestTree& source);
};


/******************************************************************************
 GetCPreprocessor

 ******************************************************************************/

inline CBCPreprocessor*
CBTestTree::GetCPreprocessor()
	const
{
	return itsCPP;
}

#endif
