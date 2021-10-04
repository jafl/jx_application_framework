/******************************************************************************
 MenuHelloDir.h

	Interface for the MenuHelloDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_MenuHelloDir
#define _H_MenuHelloDir

#include <JXWindowDirector.h>

class JXStaticText;
class JXTextMenu;

class MenuHelloDir : public JXWindowDirector
{
public:

	MenuHelloDir(JXDirector* supervisor);

	virtual ~MenuHelloDir();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXStaticText* itsText;
	JXTextMenu* itsTextMenu;

private:

	void BuildWindow();
	void HandleTextMenu(const JIndex index);

	// not allowed

	MenuHelloDir(const MenuHelloDir& source);
	const MenuHelloDir& operator=(const MenuHelloDir& source);
};

#endif
