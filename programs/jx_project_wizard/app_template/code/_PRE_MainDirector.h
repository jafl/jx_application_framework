/******************************************************************************
 <PRE>MainDirector.h

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#ifndef _H_<PRE>MainDirector
#define _H_<PRE>MainDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXTextMenu;
class JXToolBar;

class <PRE>MainDirector : public JXWindowDirector, public JPrefObject
{
public:

	<PRE>MainDirector(JXDirector* supervisor);

	virtual	~<PRE>MainDirector();

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

	<PRE>MainDirector(const <PRE>MainDirector& source);
	const <PRE>MainDirector& operator=(const <PRE>MainDirector& source);
};

#endif
