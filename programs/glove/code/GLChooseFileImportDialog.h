/******************************************************************************
 GLChooseFileImportDialog.h

	Interface for the GLChooseFileImportDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLChooseFileImportDialog
#define _H_GLChooseFileImportDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JXTextMenu;
class JXTextButton;
class JXStaticText;
class GLDataDocument;
class JString;

class GLChooseFileImportDialog : public JXDialogDirector, public JPrefObject
{
public:

	GLChooseFileImportDialog(GLDataDocument* supervisor, const JString& filename);

	virtual ~GLChooseFileImportDialog();
	
	JIndex 			GetFilterIndex();
	const JString& 	GetFileText();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;
	
private:

	JIndex 			itsFilterIndex;
	JXStaticText*	itsFileText;
	GLDataDocument*	itsDir;

// begin JXLayout

	JXTextButton* itsReloadButton;
	JXTextMenu*   itsFilterMenu;

// end JXLayout

private:

	void	BuildWindow(const JString& filename);

	// not allowed

	GLChooseFileImportDialog(const GLChooseFileImportDialog& source);
	const GLChooseFileImportDialog& operator=(const GLChooseFileImportDialog& source);
};

#endif
