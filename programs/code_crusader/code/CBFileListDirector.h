/******************************************************************************
 CBFileListDirector.h

	Interface for the CBFileListDirector class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFileListDirector
#define _H_CBFileListDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
	CBFileListDirector(istream& projInput, const JFileVersion projVers,
					   istream* setInput, const JFileVersion setVers,
					   istream* symInput, const JFileVersion symVers,
					   CBProjectDocument* supervisor, const JBoolean subProject);

	virtual ~CBFileListDirector();

	CBFileListTable*	GetFileListTable() const;

	void	StreamOut(ostream& projOutput,
					  ostream* setOutput, ostream* symOutput) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

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
