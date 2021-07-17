/******************************************************************************
 CBFileListDirector.h

	Interface for the CBFileListDirector class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFileListDirector
#define _H_CBFileListDirector

#include <JXWindowDirector.h>

class JXTextMenu;
class JXToolBar;
class JXFileListSet;
class CBFileListTable;
class CBProjectDocument;
class CBCommandMenu;

class CBFileListDirector : public JXWindowDirector
{
public:

	CBFileListDirector(CBProjectDocument* supervisor);
	CBFileListDirector(std::istream& projInput, const JFileVersion projVers,
					   std::istream* setInput, const JFileVersion setVers,
					   std::istream* symInput, const JFileVersion symVers,
					   CBProjectDocument* supervisor, const bool subProject);

	virtual ~CBFileListDirector();

	CBFileListTable*	GetFileListTable() const;

	void	StreamOut(std::ostream& projOutput,
					  std::ostream* setOutput, std::ostream* symOutput) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBProjectDocument*	itsProjDoc;
	JXFileListSet*		itsFLSet;
	CBFileListTable*	itsFLTable;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsListMenu;
	JXTextMenu*		itsProjectMenu;
	CBCommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	CBFileListDirectorX(CBProjectDocument* projDoc);
	void	BuildWindow();
	void	AdjustWindowTitle();

	void	OpenSelectedFiles() const;

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateListMenu();
	void	HandleListMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	// not allowed

	CBFileListDirector(const CBFileListDirector& source);
	const CBFileListDirector& operator=(const CBFileListDirector& source);
};


/******************************************************************************
 GetFileListTable

 ******************************************************************************/

inline CBFileListTable*
CBFileListDirector::GetFileListTable()
	const
{
	return itsFLTable;
}

#endif
