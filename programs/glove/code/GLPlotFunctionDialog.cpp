/******************************************************************************
 GLPlotFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GLPlotFunctionDialog.h"
#include "GLVarList.h"
#include "GLExprDirector.h"

#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXWindow.h>

#include <JExprParser.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLPlotFunctionDialog::GLPlotFunctionDialog
	(
	JXDirector* supervisor,
	GLVarList* list
	)
	:
	JXDialogDirector(supervisor, true)
{
	itsEditor = nullptr;
	BuildWindow();
	itsList = list;
	
	for (JIndex i = 1; i <= list->GetElementCount(); i++)
		{
		itsVarMenu->AppendItem(list->GetVariableName(i));
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLPlotFunctionDialog::~GLPlotFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLPlotFunctionDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,80, JString::empty);
	assert( window != nullptr );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 80,10, 220,20);
	assert( itsFunctionString != nullptr );

	auto* fnLabel =
		jnew JXStaticText(JGetString("fnLabel::GLPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 70,20);
	assert( fnLabel != nullptr );
	fnLabel->SetToLabel();

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::GLPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,10, 60,20);
	assert( itsEditButton != nullptr );
	itsEditButton->SetShortcuts(JGetString("itsEditButton::GLPlotFunctionDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GLPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 315,50, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLPlotFunctionDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 115,50, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::GLPlotFunctionDialog::shortcuts::JXLayout"));

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::GLPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 215,50, 70,20);
	assert( itsClearButton != nullptr );

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::GLPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 380,10, 110,20);
	assert( itsVarMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLPlotFunctionDialog"));
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
GLPlotFunctionDialog::Receive
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
 GetFunctionString

 ******************************************************************************/

const JString&
GLPlotFunctionDialog::GetFunctionString()
{
	return itsFunctionString->GetText()->GetText();
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
GLPlotFunctionDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return true;
		}

	JExprParser p(itsList);

	JFunction* f;
	if (p.Parse(itsFunctionString->GetText()->GetText(), &f))
		{
		jdelete f;
		return true;
		}
	return false;
}
