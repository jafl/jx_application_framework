/******************************************************************************
 GXPlotFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GXPlotFunctionDialog.h"
#include "GVarList.h"
#include "ExprDirector.h"

#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXWindow.h>

#include <jParseFunction.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GXPlotFunctionDialog::GXPlotFunctionDialog
	(
	JXDirector* supervisor,
	GVarList* list
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsEditor = NULL;
	BuildWindow();
	itsList = list;
	
	for (JSize i = 1; i <= list->GetElementCount(); i++)
		{
		itsVarMenu->AppendItem(list->GetVariableName(i));
		}
	
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXPlotFunctionDialog::~GXPlotFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXPlotFunctionDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 500,80, "");
	assert( window != NULL );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 80,10, 220,20);
	assert( itsFunctionString != NULL );

	JXStaticText* fnLabel =
		jnew JXStaticText(JGetString("fnLabel::GXPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 70,20);
	assert( fnLabel != NULL );
	fnLabel->SetToLabel();

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::GXPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,10, 60,20);
	assert( itsEditButton != NULL );
	itsEditButton->SetShortcuts(JGetString("itsEditButton::GXPlotFunctionDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::GXPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 315,50, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GXPlotFunctionDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GXPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 115,50, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GXPlotFunctionDialog::shortcuts::JXLayout"));

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::GXPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 215,50, 70,20);
	assert( itsClearButton != NULL );

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::GXPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 380,10, 110,20);
	assert( itsVarMenu != NULL );

// end JXLayout

	window->SetTitle("Function Window");
	SetButtons(okButton, cancelButton);
	itsVarMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsClearButton);
	ListenTo(itsEditButton);
	ListenTo(itsVarMenu);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GXPlotFunctionDialog::Receive
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
 GetFunctionString

 ******************************************************************************/

const JString&
GXPlotFunctionDialog::GetFunctionString()
{
	return itsFunctionString->GetText();
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
GXPlotFunctionDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}
	JFunction* f;
	if (JParseFunction(itsFunctionString->GetText(), itsList, &f))
		{
		return kJTrue;
		}
	return kJFalse;
}
