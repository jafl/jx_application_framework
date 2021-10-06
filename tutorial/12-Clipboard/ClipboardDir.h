/******************************************************************************
 ClipboardDir.h

	Interface for the ClipboardDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_ClipboardDir
#define _H_ClipboardDir

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

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ClipboardWidget*		itsText;
	JXTextMenu* 			itsTextMenu;
	CBStringInputDialog* 	itsDialog;

private:

	void BuildWindow();
	void HandleTextMenu(const JIndex index);
	void SetupInputDialog();
	void GetNewTextFromDialog();
};

#endif
