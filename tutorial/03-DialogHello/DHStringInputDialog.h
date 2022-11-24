/******************************************************************************
 DHStringInputDialog.h

	Interface for the DHStringInputDialog class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DHStringInputDialog
#define _H_DHStringInputDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXInputField;
class JString;

class DHStringInputDialog : public JXDialogDirector
{
public:

	DHStringInputDialog(JXWindowDirector* supervisor, const JString& str);

	JString	GetString() const;

private:

	JXInputField* itsText;

private:

	void	BuildWindow();
};

#endif
