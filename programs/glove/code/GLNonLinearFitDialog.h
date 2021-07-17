/******************************************************************************
 GLNonLinearFitDialog.h

	Interface for the GLNonLinearFitDialog class

	Copyright (C) 2000 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GLNonLinearFitDialog
#define _H_GLNonLinearFitDialog

#include <JXDialogDirector.h>

#include <JString.h>

class GLVarList;
class GLVarTable;
class JFunction;
class JXExprEditor;
class JXInputField;
class JXTextButton;
class JXTimerTask;
class JXVertPartition;

class GLNonLinearFitDialog : public JXDialogDirector
{
public:

	GLNonLinearFitDialog(JXDirector* supervisor);
	virtual ~GLNonLinearFitDialog();

	JString				GetFunctionString() const;
	JString				GetDerivativeString() const;
	const GLVarList&	GetVarList() const;
	const JString&		GetFitName() const;

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual bool	OKToDeactivate() override;

private:

	JXExprEditor*		itsFnEditor;
	JXExprEditor*		itsDerivativeEditor;
	GLVarList*			itsVarList;
	GLVarTable*			itsVarTable;
	JXTimerTask*		itsDelButtonTask;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout
// begin functionLayout

	JXInputField* itsNameInput;

// end functionLayout

// begin derivativeLayout


// end derivativeLayout

// begin variableLayout

	JXTextButton* itsNewButton;
	JXTextButton* itsDeleteButton;

// end variableLayout

private:

	void		BuildWindow();
	bool	OKToDeleteParm();

	// not allowed

	GLNonLinearFitDialog(const GLNonLinearFitDialog& source);
	const GLNonLinearFitDialog& operator=(const GLNonLinearFitDialog& source);
};

/******************************************************************************
 GetVarList (public)

 ******************************************************************************/

inline const GLVarList&
GLNonLinearFitDialog::GetVarList()
	const
{
	return *itsVarList;
}


#endif
