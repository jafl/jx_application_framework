/******************************************************************************
 CMFileListDir.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_CMFileListDir
#define _H_CMFileListDir

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXTextMenu;
class JXTextButton;
class JXFileListSet;
class JXFileListTable;
class CMLink;
class CMCommandDirector;
class CMGetSourceFileList;

class CMFileListDir : public JXWindowDirector, public JPrefObject
{
public:

	CMFileListDir(CMCommandDirector* supervisor);

	virtual	~CMFileListDir();

	JXFileListTable*	GetTable();

	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMLink*					itsLink;
	CMCommandDirector*		itsCommandDir;
	CMGetSourceFileList*	itsCmd;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionsMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout

	JXFileListSet* itsFileListSet;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMFileListDir(const CMFileListDir& source);
	const CMFileListDir& operator=(const CMFileListDir& source);
};

#endif
