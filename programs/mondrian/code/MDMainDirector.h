/******************************************************************************
 MDMainDirector.h

	Copyright © 2008 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_MDMainDirector
#define _H_MDMainDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXTextMenu;
class JXToolBar;

class MDMainDirector : public JXWindowDirector, public JPrefObject
{
public:

	MDMainDirector(JXDirector* supervisor);

	virtual	~MDMainDirector();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout

    JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);

	// not allowed

	MDMainDirector(const MDMainDirector& source);
	const MDMainDirector& operator=(const MDMainDirector& source);
};

#endif
