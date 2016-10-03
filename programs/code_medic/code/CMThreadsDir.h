/******************************************************************************
 CMThreadsDir.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_CMThreadsDir
#define _H_CMThreadsDir

#include <JXWindowDirector.h>

class JXTextButton;
class JXTextMenu;

class CMCommandDirector;
class CMThreadsWidget;

class CMThreadsDir : public JXWindowDirector
{
public:

	CMThreadsDir(CMCommandDirector* supervisor);

	virtual	~CMThreadsDir();

	virtual void			Activate();
	virtual const JString&	GetName() const;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMCommandDirector*	itsCommandDir;
	CMThreadsWidget*	itsWidget;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(CMCommandDirector* supervisor);
	void	UpdateWindowTitle(const JCharacter* binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMThreadsDir(const CMThreadsDir& source);
	const CMThreadsDir& operator=(const CMThreadsDir& source);
};

#endif
