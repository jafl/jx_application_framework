/******************************************************************************
 GXTransformFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GXTransformFunctionDialog.h"
#include "GVarList.h"
#include "ExprDirector.h"

#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXWindow.h>

#include <JFunction.h>
#include <jParseFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GXTransformFunctionDialog::GXTransformFunctionDialog
	(
	JXDirector* supervisor,
	GVarList* list,
	const JSize colCount
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsEditor = NULL;
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

GXTransformFunctionDialog::~GXTransformFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXTransformFunctionDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 580,90, "");
	assert( window != NULL );

	itsTransformButton =
		jnew JXTextButton("Transform", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,55, 80,20);
	assert( itsTransformButton != NULL );
	itsTransformButton->SetShortcuts("^M");

	itsCloseButton =
		jnew JXTextButton("Close", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 370,55, 80,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts("^[");

	itsClearButton =
		jnew JXTextButton("Clear", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,55, 80,20);
	assert( itsClearButton != NULL );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,20, 200,20);
	assert( itsFunctionString != NULL );

	itsEditButton =
		jnew JXTextButton("Edit", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 410,20, 50,20);
	assert( itsEditButton != NULL );
	itsEditButton->SetShortcuts("#E");

	itsDestMenu =
		jnew JXTextMenu("Destination:", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 115,20);
	assert( itsDestMenu != NULL );

	itsVarMenu =
		jnew JXTextMenu("Constants", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 470,20, 90,20);
	assert( itsVarMenu != NULL );

	itsColNumber =
		jnew JXStaticText("", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 65,20);
	assert( itsColNumber != NULL );
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
GXTransformFunctionDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsEditButton && message.Is(JXButton::kPushed))
		{
		assert (itsEditor == NULL);
		itsEditor = jnew ExprDirector(this, itsList, itsFunctionString->GetText());
		assert(itsEditor != NULL);
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
		assert( info != NULL );
		if (info->Successful())
			{
			itsFunctionString->SetText(itsEditor->GetString());
			}
		itsEditor = NULL;
		}
	else if (sender == itsDestMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsDestCol = selection->GetIndex();
		JString num(itsDestCol);
		JString str = "col[" + num + "] = ";
		itsColNumber->SetText(str);
		}
	else if (sender == itsVarMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
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
GXTransformFunctionDialog::GetDestination()
{
	return itsDestCol;
}

/******************************************************************************
 GetFunctionString

 ******************************************************************************/

const JString&
GXTransformFunctionDialog::GetFunctionString()
{
	return itsFunctionString->GetText();
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

JBoolean
GXTransformFunctionDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}
	JFunction* f = NULL;
	if (JParseFunction(itsFunctionString->GetText(), itsList, &f))
		{
		jdelete f;
		return kJTrue;
		}
	return kJFalse;
}
