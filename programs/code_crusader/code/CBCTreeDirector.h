/******************************************************************************
 CBCTreeDirector.h

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCTreeDirector
#define _H_CBCTreeDirector

#include "CBTreeDirector.h"

class CBCTree;
class CBEditCPPMacroDialog;

class CBCTreeDirector : public CBTreeDirector
{
public:

	CBCTreeDirector(CBProjectDocument* supervisor);
	CBCTreeDirector(std::istream& projInput, const JFileVersion projVers,
					std::istream* setInput, const JFileVersion setVers,
					std::istream* symInput, const JFileVersion symVers,
					CBProjectDocument* supervisor, const bool subProject,
					CBDirList* dirList);

	virtual ~CBCTreeDirector();

	CBCTree*	GetCTree() const;

protected:

	virtual void	UpdateTreeMenu() override;
	virtual void	HandleTreeMenu(const JIndex index) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBCTree*				itsCTree;			// not owned
	CBEditCPPMacroDialog*	itsEditCPPDialog;	// nullptr unless editing

private:

	void	CBCTreeDirectorX();

	static CBTree*	NewCTree(CBTreeDirector* director, const JSize marginWidth);
	static CBTree*	StreamInCTree(std::istream& projInput, const JFileVersion projVers,
								  std::istream* setInput, const JFileVersion setVers,
								  std::istream* symInput, const JFileVersion symVers,
								  CBTreeDirector* director,
								  const JSize marginWidth, CBDirList* dirList);
	static void		InitCTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	CBCTreeDirector(const CBCTreeDirector& source);
	const CBCTreeDirector& operator=(const CBCTreeDirector& source);
};


/******************************************************************************
 GetCTree

 ******************************************************************************/

inline CBCTree*
CBCTreeDirector::GetCTree()
	const
{
	return itsCTree;
}

#endif
