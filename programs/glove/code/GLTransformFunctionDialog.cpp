/******************************************************************************
 GLTransformFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GLTransformFunctionDialog.h"
#include "GLVarList.h"
#include "GLExprDirector.h"

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
	JXDialogDirector(supervisor, kJTrue)
{
	itsEditor = nullptr;
	BuildWindow();
	itsList = list;

	for (JSize i = 1; i <= colCount; i++)
		{
		JString str("Column ");
		JString num(i);
		str += num;
		itsDestMenu->AppendItem(str);
		}

	for (JSize i = 1; i <= list->GetElementCount(); i++)
		{
		itsVarMenu->AppendItem(list->GetVariableName(i));
		}

	itsDestCol = colCount;
	JString num(itsDestCol);
	JString str = "col[" + num + "] = ";
	itsColNumber->SetText(str);
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

	JXWindow* window = jnew JXWindow(this, 580,90, "");
	assert( window != nullptr );

	itsTransformButton =
		jnew JXTextButton("Transform", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,55, 80,20);
	assert( itsTransformButton != nullptr );
	itsTransformButton->SetShortcuts("^M");

	itsCloseButton =
		jnew JXTextButton("Close", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 370,55, 80,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts("^[");

	itsClearButton =
		jnew JXTextButton("Clear", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,55, 80,20);
	assert( itsClearButton != nullptr );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,20, 200,20);
	assert( itsFunctionString != nullptr );

	itsEditButton =
		jnew JXTextButton("Edit", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 410,20, 50,20);
	assert( itsEditButton != nullptr );
	itsEditButton->SetShortcuts("#E");

	itsDestMenu =
		jnew JXTextMenu("Destination:", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 115,20);
	assert( itsDestMenu != nullptr );

	itsVarMenu =
		jnew JXTextMenu("Constants", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 470,20, 90,20);
	assert( itsVarMenu != nullptr );

	itsColNumber =
		jnew JXStaticText("", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 65,20);
	assert( itsColNumber != nullptr );
	itsColNumber->SetToLabel();

// end JXLayout

	window->SetTitle("Transformation Window");
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
		itsEditor = jnew GLExprDirector(this, itsList, itsFunctionString->GetText());
		assert(itsEditor != nullptr);
		ListenTo(itsEditor);
		itsEditor->BeginDialog();
		}
	else if (sender == itsClearButton && message.Is(JXButton::kPushed))
		{
		itsFunctionString->SetText("");
		}
	else if (sender == itsEditor && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsFunctionString->SetText(itsEditor->GetString());
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
		itsColNumber->SetText(str);
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
	return itsFunctionString->GetText();
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

JBoolean
GLTransformFunctionDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}
	JFunction* f = nullptr;
	if (JParseFunction(itsFunctionString->GetText(), itsList, &f))
		{
		jdelete f;
		return kJTrue;
		}
	return kJFalse;
}
