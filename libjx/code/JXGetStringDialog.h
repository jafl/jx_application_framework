/******************************************************************************
 JXGetStringDialog.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXGetStringDialog
#define _H_JXGetStringDialog

#include "jx-af/jx/JXDialogDirector.h"
#include <jx-af/jcore/JString.h>

class JXInputField;

class JXGetStringDialog : public JXDialogDirector
{
public:

	JXGetStringDialog(JXDirector* supervisor, const JString& windowTitle,
					  const JString& prompt,
					  const JString& initialValue = JString::empty,
					  const bool modal = true,
					  const bool password = false);

	~JXGetStringDialog();

	const JString&	GetString() const;

	JXInputField*	GetInputField();

private:

// begin JXLayout

	JXInputField* itsInputField;

// end JXLayout

private:

	void	BuildWindow(const JString& windowTitle,
						const JString& prompt, const JString& initialValue,
						const bool password);
};


/******************************************************************************
 GetInputField

 ******************************************************************************/

inline JXInputField*
JXGetStringDialog::GetInputField()
{
	return itsInputField;
}

#endif
