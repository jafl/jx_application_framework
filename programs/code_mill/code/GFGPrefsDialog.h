/******************************************************************************
 GFGPrefsDialog.h

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GFGPrefsDialog
#define _H_GFGPrefsDialog

#include <JXDialogDirector.h>
#include <JString.h>

class JXInputField;
class JXTextButton;

class GFGPrefsDialog : public JXDialogDirector
{
public:

	GFGPrefsDialog(JXDirector* supervisor, 
					const JString& header,
					const JString& source,
					const JString& constructor,
					const JString& destructor,
					const JString& function);

	virtual	~GFGPrefsDialog();

	void	GetValues(JString* header, JString* source,
					  JString* constructor, JString* destructor,
					  JString* function);

protected:

	virtual bool	OKToDeactivate();

private:

// begin JXLayout

	JXInputField* itsHeaderInput;
	JXInputField* itsSourceInput;
	JXInputField* itsConstructorInput;
	JXInputField* itsDestructorInput;
	JXInputField* itsFunctionInput;
	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JString& header,
						const JString& source,
						const JString& constructor,
						const JString& destructor,
						const JString& function);

	// not allowed

	GFGPrefsDialog(const GFGPrefsDialog& source);
	const GFGPrefsDialog& operator=(const GFGPrefsDialog& source);
};

#endif
