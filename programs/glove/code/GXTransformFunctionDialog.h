/******************************************************************************
 GXTransformFunctionDialog.h

	Interface for the GXTransformFunctionDialog class

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GXTransformFunctionDialog
#define _H_GXTransformFunctionDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextButton;
class JXInputField;
class GVarList;
class ExprDirector;
class JXTextMenu;
class JXStaticText;
class JString;

class GXTransformFunctionDialog : public JXDialogDirector
{
public:

	GXTransformFunctionDialog(JXDirector* supervisor, GVarList* list, const JSize colCount);
	virtual ~GXTransformFunctionDialog();
	
	JIndex	GetDestination();
	const JString& GetFunctionString();
	
protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToDeactivate();

private:

// begin JXLayout

    JXTextButton*  itsTransformButton;
    JXTextButton*  itsCloseButton;
    JXTextButton*  itsClearButton;
    JXInputField*  itsFunctionString;
    JXTextButton*  itsEditButton;
    JXTextMenu*    itsDestMenu;
    JXTextMenu*    itsVarMenu;
    JXStaticText*  itsColNumber;

// end JXLayout

	ExprDirector* 	itsEditor;
	GVarList* 		itsList;
	JIndex			itsDestCol;

private:

	void	BuildWindow();

	// not allowed

	GXTransformFunctionDialog(const GXTransformFunctionDialog& source);
	const GXTransformFunctionDialog& operator=(const GXTransformFunctionDialog& source);
};

#endif
