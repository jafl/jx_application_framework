/******************************************************************************
 <PRE>MainDirector.h

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_<PRE>MainDirector
#define _H_<PRE>MainDirector

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

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
};

#endif
