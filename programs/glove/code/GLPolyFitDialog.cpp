/******************************************************************************
 GLPolyFitDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GLPolyFitDialog.h"
#include "GVarList.h"

#include "GLGlobals.h"

#include <JXExprWidget.h>
#include <JXInputField.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXWindow.h>

#include <JFunction.h>
#include <JString.h>

#include <jParseFunction.h>
#include <jAssert.h>

const JCoordinate kDeleteButtonUpdateDelay	= 1000;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLPolyFitDialog::GLPolyFitDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsVarList	= jnew GVarList();
	assert(itsVarList != NULL);

	itsVarList->AddVariable("x", 0);
	const JSize count	= 10;
	for (JIndex i = 1; i <= count; i++)
		{
		JString parm	= "a" + JString(i - 1, JString::kBase10);
		itsVarList->AddVariable(parm, 0);
		}

	BuildWindow();

}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLPolyFitDialog::~GLPolyFitDialog()
{
	// jdelete	itsVarList;	
	// I need to find a way to safely delete this.
}

/******************************************************************************


 ******************************************************************************/

void
GLPolyFitDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = jnew JXWindow(this, 380,450, "");
    assert( window != NULL );

    JXScrollbarSet* scrollbarSet =
        jnew JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 340,100);
    assert( scrollbarSet != NULL );

    itsCB[0] =
        jnew JXTextCheckbox("0 (Constant)", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,155, 110,20);
    assert( itsCB[0] != NULL );

    itsCB[1] =
        jnew JXTextCheckbox("1", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,180, 110,20);
    assert( itsCB[1] != NULL );

    itsCB[2] =
        jnew JXTextCheckbox("2", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,205, 110,20);
    assert( itsCB[2] != NULL );

    itsCB[3] =
        jnew JXTextCheckbox("3", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,230, 110,20);
    assert( itsCB[3] != NULL );

    itsCB[4] =
        jnew JXTextCheckbox("4", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,255, 110,20);
    assert( itsCB[4] != NULL );

    itsCB[5] =
        jnew JXTextCheckbox("5", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,280, 110,20);
    assert( itsCB[5] != NULL );

    itsCB[6] =
        jnew JXTextCheckbox("6", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,305, 110,20);
    assert( itsCB[6] != NULL );

    itsCB[7] =
        jnew JXTextCheckbox("7", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,330, 110,20);
    assert( itsCB[7] != NULL );

    itsCB[8] =
        jnew JXTextCheckbox("8", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,355, 110,20);
    assert( itsCB[8] != NULL );

    itsCB[9] =
        jnew JXTextCheckbox("9", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,380, 110,20);
    assert( itsCB[9] != NULL );

    JXStaticText* obj1 =
        jnew JXStaticText("Powers:", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 90,158, 65,20);
    assert( obj1 != NULL );

    JXTextButton* cancelButton =
        jnew JXTextButton("Cancel", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 25,415, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

    itsHelpButton =
        jnew JXTextButton("Help", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 155,415, 70,20);
    assert( itsHelpButton != NULL );

    JXTextButton* okButton =
        jnew JXTextButton("OK", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 285,415, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    itsNameInput =
        jnew JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 90,15, 270,20);
    assert( itsNameInput != NULL );

    JXStaticText* obj2 =
        jnew JXStaticText("Fit name:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,15, 65,20);
    assert( obj2 != NULL );

// end JXLayout


	itsFn	= 
		jnew JXExprWidget(itsVarList, 
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsFn != NULL);
	itsFn->FitToEnclosure(kJTrue, kJTrue);
	itsFn->Hide();

	ListenTo(itsHelpButton);

	const JSize count	= 10;
	for (JIndex i = 1; i <= count; i++)
		{
		ListenTo(itsCB[i-1]);
		}

	window->SetTitle("Polynomial Fit");
	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);
}


/******************************************************************************
 GetPowers

 ******************************************************************************/

void
GLPolyFitDialog::GetPowers
	(
	JArray<JIndex>* powers
	)
{
	const JSize count	= 10;
	for (JIndex i = 1; i <= count; i++)
		{
		if (itsCB[i-1]->IsChecked())
			{
			powers->AppendElement(i - 1);
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GLPolyFitDialog::Receive
	(
	JBroadcaster* 	sender, 
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		}
	else if (message.Is(JXCheckbox::kPushed))
		{
		JString fStr;
		JBoolean started	= kJFalse;
		const JSize count	= 10;
		for (JIndex i = 1; i <= count; i++)
			{
			if (itsCB[i-1]->IsChecked())
				{
				if (started)
					{
					fStr += " + ";
					}
				else
					{
					started	= kJTrue;
					}
				JString parm	= "a" + JString(i - 1, JString::kBase10);
				JString xTerm	= " * x";
				if (i > 2)
					{
					xTerm += "^" + JString(i - 1, JString::kBase10);
					}
				fStr += parm;
				if (i > 1)
					{
					fStr += xTerm;
					}
				}
			}
		if (fStr.IsEmpty())
			{
			itsFn->Hide();
			}
		else
			{
			itsFn->Show();
			JFunction* f;
			if (JParseFunction(fStr, itsVarList, &f))
				{
				itsFn->SetFunction(itsVarList, f);
				}
			}
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
GLPolyFitDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	if (Cancelled())
		{
		return kJTrue;
		}
	JString name	= itsNameInput->GetText();
	name.TrimWhitespace();
	if (name.IsEmpty())
		{
		JGetUserNotification()->ReportError("You must specify a name for this fit.");
		itsNameInput->Focus();
		return kJFalse;
		}

	JBoolean checked	= kJFalse;
	const JSize count	= 10;
	for (JIndex i = 1; i <= count; i++)
		{
		if (itsCB[i-1]->IsChecked())
			{
			checked		= kJTrue;
			break;
			}
		}

	if (!checked)
		{
		JGetUserNotification()->ReportError("You must add at least one power.");
		}

	return checked;
}

/******************************************************************************
 GetFitName (public)

 ******************************************************************************/

const JString&
GLPolyFitDialog::GetFitName()
	const
{
	return itsNameInput->GetText();
}
