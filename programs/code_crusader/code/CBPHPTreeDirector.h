/******************************************************************************
 CBPHPTreeDirector.h

	Copyright © 2014 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPHPTreeDirector
#define _H_CBPHPTreeDirector

#include "CBTreeDirector.h"

class CBPHPTree;

class CBPHPTreeDirector : public CBTreeDirector
{
public:

	CBPHPTreeDirector(CBProjectDocument* supervisor);
	CBPHPTreeDirector(std::istream& projInput, const JFileVersion projVers,
					  std::istream* setInput, const JFileVersion setVers,
					  std::istream* symInput, const JFileVersion symVers,
					  CBProjectDocument* supervisor, const bool subProject);

	virtual ~CBPHPTreeDirector();

	CBPHPTree*	GetPHPTree() const;

protected:

	virtual void	UpdateTreeMenu();
	virtual void	HandleTreeMenu(const JIndex index);

private:

	CBPHPTree*	itsPHPTree;	// not owned

private:

	void	CBPHPTreeDirectorX();

	static CBTree*	NewPHPTree(CBTreeDirector* director, const JSize marginWidth);
	static CBTree*	StreamInPHPTree(std::istream& projInput, const JFileVersion projVers,
									std::istream* setInput, const JFileVersion setVers,
									std::istream* symInput, const JFileVersion symVers,
									CBTreeDirector* director,
									const JSize marginWidth, CBDirList* dirList);
	static void		InitPHPTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	CBPHPTreeDirector(const CBPHPTreeDirector& source);
	const CBPHPTreeDirector& operator=(const CBPHPTreeDirector& source);
};


/******************************************************************************
 GetPHPTree

 ******************************************************************************/

inline CBPHPTree*
CBPHPTreeDirector::GetPHPTree()
	const
{
	return itsPHPTree;
}

#endif
