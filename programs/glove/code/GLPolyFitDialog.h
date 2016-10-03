/******************************************************************************
 GLPolyFitDialog.h

	Interface for the GLPolyFitDialog class

	Copyright (C) 2000 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GLPolyFitDialog
#define _H_GLPolyFitDialog

#include <JXDialogDirector.h>

#include <JArray.h>

class GVarList;
class JString;
class JXExprWidget;
class JXInputField;
class JXTextButton;
class JXTextCheckbox;

class GLPolyFitDialog : public JXDialogDirector
{
public:

	GLPolyFitDialog(JXDirector* supervisor);
	virtual ~GLPolyFitDialog();

	void			GetPowers(JArray<JIndex>* powers);
	const JString&	GetFitName() const;

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToDeactivate();

private:

	JXExprWidget*		itsFn;
	GVarList*			itsVarList;
	JXTextCheckbox*		itsCB[10];

// begin JXLayout

    JXTextButton* itsHelpButton;
    JXInputField* itsNameInput;

// end JXLayout

private:

	void		BuildWindow();

	// not allowed

	GLPolyFitDialog(const GLPolyFitDialog& source);
	const GLPolyFitDialog& operator=(const GLPolyFitDialog& source);
};

#endif
