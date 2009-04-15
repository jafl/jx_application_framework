/******************************************************************************
 ExprDirector.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "ExprDirector.h"
#include "GVarList.h"
#include <JXTextButton.h>
#include <JFunction.h>
#include <JFunctionWithVar.h>
#include <JExprNodeList.h>
#include <JFunctionType.h>
#include <JString.h>
#include <jParseFunction.h>
#include <JXExprEditor.h>
#include <JXExprEditorSet.h>
#include <JXWindow.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ExprDirector::ExprDirector
	(
	JXDirector* supervisor,
	const GVarList* list,
	const JCharacter* function
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	JXWindow* window = new JXWindow(this, 300,240, "Function Editor");
	assert( window != NULL );
	SetWindow(window);
    
    JXExprEditorSet* set = 
    	new JXExprEditorSet(list, &itsEditor, window,
    	JXWidget::kHElastic, JXWidget::kVElastic,0,0,300,200);
    assert (set != NULL);
    assert (itsEditor != NULL);

	JString funStr(function);
    
	if ((!funStr.IsEmpty()) && JParseFunction(funStr,list,&itsFunction, kJTrue))
		{
		itsEditor->SetFunction(list,itsFunction);
		}
	JXTextButton* ok = new JXTextButton("Return", window, 
						JXWidget::kHElastic, JXWidget::kVElastic,60,210,70,20);
	assert (ok != NULL);
	ok->SetShortcuts("^M");
	JXTextButton* cancel = new JXTextButton("Cancel", window, 
						JXWidget::kHElastic, JXWidget::kVElastic,170,210,70,20);
	assert (cancel != NULL);
    cancel->SetShortcuts("^[");
	SetButtons(ok,cancel);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExprDirector::~ExprDirector()
{
//	delete itsFunction;
}

/******************************************************************************
 GetString

 ******************************************************************************/

JString
ExprDirector::GetString()
{
	const JFunction* f = itsEditor->GetFunction();
	return f->Print();
}


