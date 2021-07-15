/******************************************************************************
 GLFitModuleDialog.h

	Interface for the GLFitModuleDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLFitModuleDialog
#define _H_GLFitModuleDialog

#include <JXDialogDirector.h>

class JXTextMenu;
class JXTextButton;

class GLFitModuleDialog : public JXDialogDirector
{
public:

	GLFitModuleDialog(JXWindowDirector* supervisor);

	virtual ~GLFitModuleDialog();

	JIndex GetFilterIndex();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex itsFilterIndex;

// begin JXLayout

	JXTextMenu*   itsFilterMenu;
	JXTextButton* itsOKButton;
	JXTextButton* itsReloadButton;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GLFitModuleDialog(const GLFitModuleDialog& source);
	const GLFitModuleDialog& operator=(const GLFitModuleDialog& source);
};

#endif
