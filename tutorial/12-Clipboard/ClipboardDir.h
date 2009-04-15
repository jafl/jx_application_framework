/******************************************************************************
 ClipboardDir.h

	Interface for the ClipboardDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_ClipboardDir
#define _H_ClipboardDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class ClipboardWidget;
class JXTextMenu;
class CBStringInputDialog;

class ClipboardDir : public JXWindowDirector
{
public:

	ClipboardDir(JXDirector* supervisor);

	virtual ~ClipboardDir();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	ClipboardWidget*		itsText;
	JXTextMenu* 			itsTextMenu;
	CBStringInputDialog* 	itsDialog;

private:

	void BuildWindow();
	void HandleTextMenu(const JIndex index);
	void SetupInputDialog();
	void GetNewTextFromDialog();

	// not allowed

	ClipboardDir(const ClipboardDir& source);
	const ClipboardDir& operator=(const ClipboardDir& source);
};

#endif
