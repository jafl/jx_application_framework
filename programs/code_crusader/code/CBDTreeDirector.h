/******************************************************************************
 CBDTreeDirector.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDTreeDirector
#define _H_CBDTreeDirector

#include "CBTreeDirector.h"

class CBDTree;

class CBDTreeDirector : public CBTreeDirector
{
public:

	CBDTreeDirector(CBProjectDocument* supervisor);
	CBDTreeDirector(std::istream& projInput, const JFileVersion projVers,
					std::istream* setInput, const JFileVersion setVers,
					std::istream* symInput, const JFileVersion symVers,
					CBProjectDocument* supervisor, const bool subProject);

	virtual ~CBDTreeDirector();

	CBDTree*	GetDTree() const;

protected:

	virtual void	UpdateTreeMenu() override;
	virtual void	HandleTreeMenu(const JIndex index) override;

private:

	CBDTree*	itsDTree;			// not owned

private:

	void	CBDTreeDirectorX();

	static CBTree*	NewDTree(CBTreeDirector* director, const JSize marginWidth);
	static CBTree*	StreamInDTree(std::istream& projInput, const JFileVersion projVers,
								  std::istream* setInput, const JFileVersion setVers,
								  std::istream* symInput, const JFileVersion symVers,
								  CBTreeDirector* director,
								  const JSize marginWidth, CBDirList* dirList);
	static void		InitDTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	CBDTreeDirector(const CBDTreeDirector& source);
	const CBDTreeDirector& operator=(const CBDTreeDirector& source);
};


/******************************************************************************
 GetDTree

 ******************************************************************************/

inline CBDTree*
CBDTreeDirector::GetDTree()
	const
{
	return itsDTree;
}

#endif
