/******************************************************************************
 DHStringInputDialog.h

	Interface for the DHStringInputDialog class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DHStringInputDialog
#define _H_DHStringInputDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXInputField;

class DHStringInputDialog : public JXModalDialogDirector
{
public:

	DHStringInputDialog(const JString& str);

	JString	GetString() const;

private:

	JXInputField* itsText;

private:

	void	BuildWindow();
};

#endif
