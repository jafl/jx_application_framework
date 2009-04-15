/******************************************************************************
 SCEditFnDialog.h

	Interface for the SCEditFnDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCEditFnDialog
#define _H_SCEditFnDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class SCCircuitVarList;
class JXExprEditor;
class JXExprInput;

class SCEditFnDialog : public JXDialogDirector
{
public:

	SCEditFnDialog(JXWindowDirector* supervisor,
				   SCCircuitVarList* varList, const JIndex varIndex);

	virtual ~SCEditFnDialog();

protected:

	virtual JBoolean	OKToDeactivate();

private:

	SCCircuitVarList*	itsVarList;		// owned by supervisor
	JIndex				itsVarIndex;

	JXExprEditor*	itsExprWidget;

// begin JXLayout

    JXExprInput* itsVarName;

// end JXLayout

private:

	void	BuildWindow(const SCCircuitVarList* varList, const JIndex varIndex);

	// not allowed

	SCEditFnDialog(const SCEditFnDialog& source);
	const SCEditFnDialog& operator=(const SCEditFnDialog& source);
};

#endif
