/******************************************************************************
 GLExprDirector.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GLExprDirector.h"
#include "GLVarList.h"
#include <JXTextButton.h>
#include <JFunction.h>
#include <JXExprEditor.h>
#include <JXExprEditorSet.h>
#include <JXWindow.h>
#include <JExprParser.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLExprDirector::GLExprDirector
	(
	JXDirector* supervisor,
	const GLVarList* list,
	const JString& function
	)
	:
	JXDialogDirector(supervisor, true)
{
	auto* window = jnew JXWindow(this, 300,240, JGetString("WindowTitle::GLExprDirector"));
	assert( window != nullptr );

	auto* set =
		jnew JXExprEditorSet(list, &itsEditor, window,
		JXWidget::kHElastic, JXWidget::kVElastic,0,0,300,200);
	assert (set != nullptr);
	assert (itsEditor != nullptr);

	JExprParser p(itsEditor);

	JFunction* f;
	if (!function.IsEmpty() && p.Parse(function, &f))
		{
		itsEditor->SetFunction(list, f);
		}

	auto* ok = jnew JXTextButton(JGetString("OKLabel::JXGlobal"), window,
						JXWidget::kHElastic, JXWidget::kVElastic,60,210,70,20);
	assert (ok != nullptr);
	ok->SetShortcuts(JGetString("OKShortcut::JXGlobal"));

	auto* cancel = jnew JXTextButton(JGetString("CancelLabel::JXGlobal"), window,
						JXWidget::kHElastic, JXWidget::kVElastic,170,210,70,20);
	assert (cancel != nullptr);
	cancel->SetShortcuts(JGetString("CancelShortcut::JXGlobal"));

	SetButtons(ok,cancel);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLExprDirector::~GLExprDirector()
{
}

/******************************************************************************
 GetString

 ******************************************************************************/

JString
GLExprDirector::GetString()
{
	const JFunction* f = itsEditor->GetFunction();
	return f->Print();
}
