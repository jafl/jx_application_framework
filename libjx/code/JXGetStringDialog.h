/******************************************************************************
 JXGetStringDialog.h

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetStringDialog
#define _H_JXGetStringDialog

#include <JXDialogDirector.h>
#include <JString.h>

class JXInputField;

class JXGetStringDialog : public JXDialogDirector
{
public:

	JXGetStringDialog(JXDirector* supervisor, const JString& windowTitle,
					  const JString& prompt,
					  const JString& initialValue = JString::empty,
					  const JBoolean modal = kJTrue,
					  const JBoolean password = kJFalse);

	virtual ~JXGetStringDialog();

	const JString&	GetString() const;

	JXInputField*	GetInputField();

private:

// begin JXLayout

	JXInputField* itsInputField;

// end JXLayout

private:

	void	BuildWindow(const JString& windowTitle,
						const JString& prompt, const JString& initialValue,
						const JBoolean password);

	// not allowed

	JXGetStringDialog(const JXGetStringDialog& source);
	const JXGetStringDialog& operator=(const JXGetStringDialog& source);
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
