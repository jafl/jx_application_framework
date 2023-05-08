/******************************************************************************
 MainDirector.h

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_MainDirector
#define _H_MainDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextMenu;
class JXToolBar;

class MainDirector : public JXWindowDirector, public JPrefObject
{
public:

	MainDirector(JXDirector* supervisor);

	~MainDirector() override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

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

	void	HandlePrefsMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};

#endif
