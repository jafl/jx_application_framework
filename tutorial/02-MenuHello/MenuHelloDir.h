/******************************************************************************
 MenuHelloDir.h

	Interface for the MenuHelloDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_MenuHelloDir
#define _H_MenuHelloDir

#include <jx-af/jx/JXWindowDirector.h>

class JXStaticText;
class JXTextMenu;

class MenuHelloDir : public JXWindowDirector
{
public:

	MenuHelloDir(JXDirector* supervisor);

	~MenuHelloDir() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXStaticText*	itsText;
	JXTextMenu*		itsTextMenu;

private:

	void BuildWindow();
	void HandleTextMenu(const JIndex index);
};

#endif
