/******************************************************************************
 GXPlotFunctionDialog.h

	Interface for the GXPlotFunctionDialog class

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GXPlotFunctionDialog
#define _H_GXPlotFunctionDialog

#include <JXDialogDirector.h>

class JXTextButton;
class JXInputField;
class GVarList;
class ExprDirector;
class JXTextMenu;
class JString;

class GXPlotFunctionDialog : public JXDialogDirector
{
public:

	GXPlotFunctionDialog(JXDirector* supervisor, GVarList* list);
	virtual ~GXPlotFunctionDialog();
	
	const JString& GetFunctionString();
	
protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToDeactivate();

private:

// begin JXLayout

	JXInputField* itsFunctionString;
	JXTextButton* itsEditButton;
	JXTextButton* itsClearButton;
	JXTextMenu*   itsVarMenu;

// end JXLayout

	ExprDirector* 	itsEditor;
	GVarList* 		itsList;

private:

	void	BuildWindow();

	// not allowed

	GXPlotFunctionDialog(const GXPlotFunctionDialog& source);
	const GXPlotFunctionDialog& operator=(const GXPlotFunctionDialog& source);
};

#endif
