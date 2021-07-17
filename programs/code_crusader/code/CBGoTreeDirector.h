/******************************************************************************
 CBGoTreeDirector.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBGoTreeDirector
#define _H_CBGoTreeDirector

#include "CBTreeDirector.h"

class CBGoTree;

class CBGoTreeDirector : public CBTreeDirector
{
public:

	CBGoTreeDirector(CBProjectDocument* supervisor);
	CBGoTreeDirector(std::istream& projInput, const JFileVersion projVers,
					 std::istream* setInput, const JFileVersion setVers,
					 std::istream* symInput, const JFileVersion symVers,
					 CBProjectDocument* supervisor, const bool subProject);

	virtual ~CBGoTreeDirector();

	CBGoTree*	GetGoTree() const;

protected:

	virtual void	UpdateTreeMenu();
	virtual void	HandleTreeMenu(const JIndex index);

private:

	CBGoTree*	itsGoTree;	// not owned

private:

	void	CBGoTreeDirectorX();

	static CBTree*	NewGoTree(CBTreeDirector* director, const JSize marginWidth);
	static CBTree*	StreamInGoTree(std::istream& projInput, const JFileVersion projVers,
								   std::istream* setInput, const JFileVersion setVers,
								   std::istream* symInput, const JFileVersion symVers,
								   CBTreeDirector* director,
								   const JSize marginWidth, CBDirList* dirList);
	static void		InitGoTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	CBGoTreeDirector(const CBGoTreeDirector& source);
	const CBGoTreeDirector& operator=(const CBGoTreeDirector& source);
};


/******************************************************************************
 GetGoTree

 ******************************************************************************/

inline CBGoTree*
CBGoTreeDirector::GetGoTree()
	const
{
	return itsGoTree;
}

#endif
