/******************************************************************************
 DHStringInputDialog.h

	Interface for the DHStringInputDialog class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DHStringInputDialog
#define _H_DHStringInputDialog

#include <JXDialogDirector.h>

class JXInputField;
class JString;

class DHStringInputDialog : public JXDialogDirector
{
public:

	DHStringInputDialog(JXWindowDirector* supervisor, const JCharacter* str);

	virtual ~DHStringInputDialog();

	JString	GetString() const;

private:

	JXInputField* itsText;

private:

	void	BuildWindow();
};

#endif
