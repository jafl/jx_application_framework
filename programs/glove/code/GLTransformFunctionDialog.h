/******************************************************************************
 GLTransformFunctionDialog.h

	Interface for the GLTransformFunctionDialog class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GLTransformFunctionDialog
#define _H_GLTransformFunctionDialog

#include <JXDialogDirector.h>

class JXTextButton;
class JXInputField;
class GLVarList;
class GLExprDirector;
class JXTextMenu;
class JXStaticText;
class JString;

class GLTransformFunctionDialog : public JXDialogDirector
{
public:

	GLTransformFunctionDialog(JXDirector* supervisor, GLVarList* list, const JSize colCount);
	virtual ~GLTransformFunctionDialog();

	JIndex	GetDestination();
	const JString& GetFunctionString();

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToDeactivate() override;

private:

// begin JXLayout

	JXTextButton* itsTransformButton;
	JXTextButton* itsCloseButton;
	JXTextButton* itsClearButton;
	JXInputField* itsFunctionString;
	JXTextButton* itsEditButton;
	JXTextMenu*   itsDestMenu;
	JXTextMenu*   itsVarMenu;
	JXStaticText* itsColNumber;

// end JXLayout

	GLExprDirector* itsEditor;
	GLVarList* 		itsList;
	JIndex			itsDestCol;

private:

	void	BuildWindow();

	// not allowed

	GLTransformFunctionDialog(const GLTransformFunctionDialog& source);
	const GLTransformFunctionDialog& operator=(const GLTransformFunctionDialog& source);
};

#endif
