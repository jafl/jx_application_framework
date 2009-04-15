/******************************************************************************
 CBStringInputDialog.h

	Interface for the CBStringInputDialog class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_CBStringInputDialog
#define _H_CBStringInputDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXInputField;
class JString;

class CBStringInputDialog : public JXDialogDirector
{
public:

	CBStringInputDialog(JXWindowDirector* supervisor, const JCharacter* str);

	virtual ~CBStringInputDialog();

	JString	GetString() const;

private:

    JXInputField* itsText;

private:

	void	BuildWindow();

	// not allowed

	CBStringInputDialog(const CBStringInputDialog& source);
	const CBStringInputDialog& operator=(const CBStringInputDialog& source);
};

#endif
