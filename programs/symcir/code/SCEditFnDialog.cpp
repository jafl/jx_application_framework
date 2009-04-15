/******************************************************************************
 SCEditFnDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCEditFnDialog.h"
#include "SCCircuitVarList.h"

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXExprInput.h>
#include <JXExprEditorSet.h>
#include <JXExprEditor.h>
#include <jXGlobals.h>

#include <JFunction.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCEditFnDialog::SCEditFnDialog
	(
	JXWindowDirector*	supervisor,
	SCCircuitVarList*	varList,
	const JIndex		varIndex
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	assert( varList->IsFunction(varIndex) );

	itsVarList  = varList;
	itsVarIndex = varIndex;
	BuildWindow(varList, varIndex);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCEditFnDialog::~SCEditFnDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SCEditFnDialog::BuildWindow
	(
	const SCCircuitVarList*	varList,
	const JIndex			varIndex
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 330,230, "");
    assert( window != NULL );
    SetWindow(window);

    itsVarName =
        new JXExprInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 110,15, 200,20);
    assert( itsVarName != NULL );

    JXExprEditorSet* obj1 =
        new JXExprEditorSet(varList, &itsExprWidget, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,50, 330,130);
    assert( obj1 != NULL );

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 50,200, 70,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 199,199, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXStaticText* obj2 =
        new JXStaticText("Variable name:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,15, 100,20);
    assert( obj2 != NULL );

// end JXLayout

	window->SetTitle("Edit function");
	SetButtons(okButton, cancelButton);

	itsVarName->SetVarName(varList->GetVariableName(varIndex));
	itsVarName->SetIsRequired();

	const JFunction* f;
	const JBoolean ok = varList->GetFunction(varIndex, &f);
	assert( ok );
	JFunction* fCopy = f->Copy();
	itsExprWidget->SetFunction(varList, fCopy);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
SCEditFnDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else if (itsExprWidget->ContainsUIF())
		{
		(JGetUserNotification())->ReportError("Please finish entering the function.");
		itsExprWidget->Focus();
		return kJFalse;
		}

	const JString varName = itsVarName->GetVarName();
	if (!itsVarList->SetVariableName(itsVarIndex, varName))
		{
		itsVarName->Focus();
		return kJFalse;
		}
	else
		{
		const JBoolean ok =
			itsVarList->SetFunction(itsVarIndex, *(itsExprWidget->GetFunction()));
		assert( ok );
		return kJTrue;
		}
}
