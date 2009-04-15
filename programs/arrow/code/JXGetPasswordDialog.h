/******************************************************************************
 JXGetPasswordDialog.h

	Interface for the JXGetPasswordDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXGetPasswordDialog
#define _H_JXGetPasswordDialog

#include <JXDialogDirector.h>

class JXPasswordInput;
class JString;

class JXGetPasswordDialog : public JXDialogDirector
{
public:

	JXGetPasswordDialog(JXDirector* supervisor, const JCharacter* prompt);

	virtual ~JXGetPasswordDialog();

	const JString& GetPassword();

protected:

private:

// begin JXLayout

    JXPasswordInput* itsInput;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* prompt);

	// not allowed

	JXGetPasswordDialog(const JXGetPasswordDialog& source);
	const JXGetPasswordDialog& operator=(const JXGetPasswordDialog& source);
};

#endif
