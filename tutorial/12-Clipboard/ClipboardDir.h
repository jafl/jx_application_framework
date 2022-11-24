/******************************************************************************
 ClipboardDir.h

	Interface for the ClipboardDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_ClipboardDir
#define _H_ClipboardDir

#include <jx-af/jx/JXWindowDirector.h>

class ClipboardWidget;
class JXTextMenu;
class JXGetStringDialog;

class ClipboardDir : public JXWindowDirector
{
public:

	ClipboardDir(JXDirector* supervisor);

	~ClipboardDir() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ClipboardWidget*	itsText;
	JXTextMenu* 		itsTextMenu;
	JXGetStringDialog*	itsDialog;

private:

	void BuildWindow();
	void HandleTextMenu(const JIndex index);
	void SetupInputDialog();
	void GetNewTextFromDialog();
};

#endif
