/******************************************************************************
 GXPlotFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GXPlotFunctionDialog.h"
#include "GVarList.h"
#include "ExprDirector.h"

#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXWindow.h>

#include <jParseFunction.h>
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

    JXWindow* window = new JXWindow(this, 500,80, "");
    assert( window != NULL );
    SetWindow(window);

    itsFunctionString =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 80,10, 220,20);
    assert( itsFunctionString != NULL );

    JXStaticText* obj1 =
        new JXStaticText("Function:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,13, 60,20);
    assert( obj1 != NULL );

    itsEditButton =
        new JXTextButton("Edit", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 310,10, 60,20);
    assert( itsEditButton != NULL );
    itsEditButton->SetShortcuts("#E");

    itsVarMenu =
        new JXTextMenu("Constants", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 380,10, 110,20);
    assert( itsVarMenu != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 115,50, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 315,50, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

    itsClearButton =
        new JXTextButton("Clear", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 215,50, 70,20);
    assert( itsClearButton != NULL );

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
		itsEditor = new ExprDirector(this, itsList, itsFunctionString->GetText());
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
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
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
			dynamic_cast(const JXMenu::ItemSelected*, &message);
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
