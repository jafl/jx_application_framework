/******************************************************************************
 GLTransformFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GLTransformFunctionDialog.h"
#include "GLVarList.h"
#include "GLExprDirector.h"
#include "GLGlobals.h"

#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXWindow.h>

#include <JFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLTransformFunctionDialog::GLTransformFunctionDialog
	(
	JXDirector* supervisor,
	GLVarList* list,
	const JSize colCount
	)
	:
	JXDialogDirector(supervisor, true)
{
	itsEditor = nullptr;
	BuildWindow();
	itsList = list;

	GLBuildColumnMenus("Column::GLGlobal", colCount, itsDestMenu, nullptr);

	for (JSize i = 1; i <= list->GetElementCount(); i++)
		{
		itsVarMenu->AppendItem(list->GetVariableName(i));
		}

	itsDestCol = colCount;
	JString num((JUInt64) itsDestCol);
	JString str = "col[" + num + "] = ";
	itsColNumber->GetText()->SetText(str);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLTransformFunctionDialog::~GLTransformFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLTransformFunctionDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 580,90, JString::empty);
	assert( window != nullptr );

	itsTransformButton =
		jnew JXTextButton(JGetString("itsTransformButton::GLTransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 370,55, 80,20);
	assert( itsTransformButton != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::GLTransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,55, 80,20);
	assert( itsCloseButton != nullptr );

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::GLTransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,55, 80,20);
	assert( itsClearButton != nullptr );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,20, 200,20);
	assert( itsFunctionString != nullptr );

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::GLTransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 410,20, 50,20);
	assert( itsEditButton != nullptr );
	itsEditButton->SetShortcuts(JGetString("itsEditButton::GLTransformFunctionDialog::shortcuts::JXLayout"));

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::GLTransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 115,20);
	assert( itsDestMenu != nullptr );

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::GLTransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 470,20, 90,20);
	assert( itsVarMenu != nullptr );

	itsColNumber =
		jnew JXStaticText(JGetString("itsColNumber::GLTransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 65,20);
	assert( itsColNumber != nullptr );
	itsColNumber->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLTransformFunctionDialog"));
	SetButtons(itsTransformButton, itsCloseButton);
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsDestMenu->SetPopupArrowPosition(JXMenu::kArrowAtLeft);
	itsVarMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsClearButton);
	ListenTo(itsEditButton);
	ListenTo(itsDestMenu);
	ListenTo(itsVarMenu);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GLTransformFunctionDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsEditButton && message.Is(JXButton::kPushed))
		{
		assert (itsEditor == nullptr);
		itsEditor = jnew GLExprDirector(this, itsList, itsFunctionString->GetText()->GetText());
		assert(itsEditor != nullptr);
		ListenTo(itsEditor);
		itsEditor->BeginDialog();
		}
	else if (sender == itsClearButton && message.Is(JXButton::kPushed))
		{
		itsFunctionString->GetText()->SetText(JString::empty);
		}
	else if (sender == itsEditor && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsFunctionString->GetText()->SetText(itsEditor->GetString());
			}
		itsEditor = nullptr;
		}
	else if (sender == itsDestMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsDestCol = selection->GetIndex();
		JString num((JUInt64) itsDestCol);
		JString str = "col[" + num + "] = ";
		itsColNumber->GetText()->SetText(str);
		}
	else if (sender == itsVarMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		JIndex index = selection->GetIndex();
		JString str = itsVarMenu->GetItemText(index);
		itsFunctionString->Paste(str);
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetDestination

 ******************************************************************************/

JIndex
GLTransformFunctionDialog::GetDestination()
{
	return itsDestCol;
}

/******************************************************************************
 GetFunctionString

 ******************************************************************************/

const JString&
GLTransformFunctionDialog::GetFunctionString()
{
	return itsFunctionString->GetText()->GetText();
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

bool
GLTransformFunctionDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return true;
		}
	JFunction* f = nullptr;
	if (JParseFunction(itsFunctionString->GetText(), itsList, &f))
		{
		jdelete f;
		return true;
		}
	return false;
}
