/******************************************************************************
 SCAddFnDialog.h

	Interface for the SCAddFnDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCAddFnDialog
#define _H_SCAddFnDialog

#include <JXDialogDirector.h>

class SCCircuitVarList;
class JXExprEditor;
class JXExprInput;

class SCAddFnDialog : public JXDialogDirector
{
public:

	SCAddFnDialog(JXWindowDirector* supervisor, SCCircuitVarList* varList);

	virtual ~SCAddFnDialog();

protected:

	virtual JBoolean	OKToDeactivate();

private:

	SCCircuitVarList*	itsVarList;		// owned by supervisor
	JXExprEditor*		itsExprWidget;

// begin JXLayout

	JXExprInput* itsVarName;

// end JXLayout

private:

	void	BuildWindow(const SCCircuitVarList* varList);

	// not allowed

	SCAddFnDialog(const SCAddFnDialog& source);
	const SCAddFnDialog& operator=(const SCAddFnDialog& source);
};

#endif
