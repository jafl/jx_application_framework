/******************************************************************************
 JXGetStringDialog.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetStringDialog
#define _H_JXGetStringDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXInputField;

class JXGetStringDialog : public JXDialogDirector
{
public:

	JXGetStringDialog(JXDirector* supervisor, const JCharacter* windowTitle,
					  const JCharacter* prompt, const JCharacter* initialValue,
					  const JBoolean modal = kJTrue);

	virtual ~JXGetStringDialog();

	const JString&	GetString() const;

protected:

	JXInputField*	GetInputField();

private:

// begin JXLayout

    JXInputField* itsInputField;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* windowTitle,
						const JCharacter* prompt, const JCharacter* initialValue);

	// not allowed

	JXGetStringDialog(const JXGetStringDialog& source);
	const JXGetStringDialog& operator=(const JXGetStringDialog& source);
};


/******************************************************************************
 GetInputField (protected)

 ******************************************************************************/

inline JXInputField*
JXGetStringDialog::GetInputField()
{
	return itsInputField;
}

#endif
