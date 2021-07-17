/******************************************************************************
 CBJavaTreeDirector.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBJavaTreeDirector
#define _H_CBJavaTreeDirector

#include "CBTreeDirector.h"

class CBJavaTree;

class CBJavaTreeDirector : public CBTreeDirector
{
public:

	CBJavaTreeDirector(CBProjectDocument* supervisor);
	CBJavaTreeDirector(std::istream& projInput, const JFileVersion projVers,
					   std::istream* setInput, const JFileVersion setVers,
					   std::istream* symInput, const JFileVersion symVers,
					   CBProjectDocument* supervisor, const bool subProject);

	virtual ~CBJavaTreeDirector();

	CBJavaTree*	GetJavaTree() const;

protected:

	virtual void	UpdateTreeMenu();
	virtual void	HandleTreeMenu(const JIndex index);

private:

	CBJavaTree*	itsJavaTree;	// not owned

private:

	void	CBJavaTreeDirectorX();

	static CBTree*	NewJavaTree(CBTreeDirector* director, const JSize marginWidth);
	static CBTree*	StreamInJavaTree(std::istream& projInput, const JFileVersion projVers,
									 std::istream* setInput, const JFileVersion setVers,
									 std::istream* symInput, const JFileVersion symVers,
									 CBTreeDirector* director,
									 const JSize marginWidth, CBDirList* dirList);
	static void		InitJavaTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	CBJavaTreeDirector(const CBJavaTreeDirector& source);
	const CBJavaTreeDirector& operator=(const CBJavaTreeDirector& source);
};


/******************************************************************************
 GetJavaTree

 ******************************************************************************/

inline CBJavaTree*
CBJavaTreeDirector::GetJavaTree()
	const
{
	return itsJavaTree;
}

#endif
