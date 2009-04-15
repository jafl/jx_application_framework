/******************************************************************************
 DialogHelloDir.h

	Interface for the DialogHelloDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DialogHelloDir
#define _H_DialogHelloDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXStaticText;
class JXTextMenu;
class DHStringInputDialog;

class DialogHelloDir : public JXWindowDirector
{
public:

	DialogHelloDir(JXDirector* supervisor);

	virtual ~DialogHelloDir();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXStaticText* itsText;
	JXTextMenu* itsTextMenu;
	DHStringInputDialog* itsDialog;

private:

	void BuildWindow();
	void HandleTextMenu(const JIndex index);
	void SetupInputDialog();
	void GetNewTextFromDialog();

	// not allowed

	DialogHelloDir(const DialogHelloDir& source);
	const DialogHelloDir& operator=(const DialogHelloDir& source);
};

#endif
