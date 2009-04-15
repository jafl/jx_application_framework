/******************************************************************************
 GMMessageFindDialog.h

	Interface for the GMMessageFindDialog class

	Copyright @ 2001 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GMMessageFindDialog
#define _H_GMMessageFindDialog

#include <JXDialogDirector.h>

class JXInputField;
class JXTextCheckbox;
class JXTextButton;

class JString;

class GMessageTableDir;

class GMMessageFindDialog : public JXDialogDirector
{
public:

	GMMessageFindDialog(JXDirector* supervisor);

	virtual ~GMMessageFindDialog();

	const JString& 	GetSearchString() const;
	JBoolean 		SearchMessage() const;
	

protected:

private:

// begin JXLayout

    JXInputField*   itsSearchInput;
    JXTextCheckbox* itsMessageBodyCB;
    JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GMMessageFindDialog(const GMMessageFindDialog& source);
	const GMMessageFindDialog& operator=(const GMMessageFindDialog& source);
};

#endif
