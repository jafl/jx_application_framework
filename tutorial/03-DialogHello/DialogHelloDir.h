/******************************************************************************
 DialogHelloDir.h

	Interface for the DialogHelloDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DialogHelloDir
#define _H_DialogHelloDir

#include <jx-af/jx/JXWindowDirector.h>

class JXStaticText;
class JXTextMenu;

class DialogHelloDir : public JXWindowDirector
{
public:

	DialogHelloDir(JXDirector* supervisor);

	~DialogHelloDir() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXStaticText* itsText;
	JXTextMenu* itsTextMenu;

private:

	void BuildWindow();
	void HandleTextMenu(const JIndex index);
};

#endif
