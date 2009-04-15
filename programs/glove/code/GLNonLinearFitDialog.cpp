/******************************************************************************
 GLNonLinearFitDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 2000 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GLNonLinearFitDialog.h"
#include "GVarList.h"
#include "GLVarTable.h"

#include "GLGlobals.h"

#include <JXApplication.h>
#include <JXExprEditor.h>
#include <JXExprEditorSet.h>
#include <JXInputField.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTimerTask.h>
#include <JXVertPartition.h>
#include <JXWindow.h>

#include <JExprNodeList.h>
#include <JFunction.h>
#include <JFunctionType.h>
#include <JFunctionWithVar.h>
#include <JString.h>

#include <jParseFunction.h>
#include <jAssert.h>

const JCoordinate kDeleteButtonUpdateDelay	= 1000;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLNonLinearFitDialog::GLNonLinearFitDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsVarList	= new GVarList();
	assert(itsVarList != NULL);

	itsVarList->AddVariable("x", 0);

	BuildWindow();

	itsDelButtonTask	= new JXTimerTask(kDeleteButtonUpdateDelay);
	assert(itsDelButtonTask != NULL);
	ListenTo(itsDelButtonTask);
	JXGetApplication()->InstallIdleTask(itsDelButtonTask);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLNonLinearFitDialog::~GLNonLinearFitDialog()
{
	// delete	itsVarList;	
	// I need to find a way to safely delete this.
}

/******************************************************************************


 ******************************************************************************/

void
GLNonLinearFitDialog::BuildWindow()
{
	const JCoordinate kNameInputHeight	= 20;
	const JCoordinate kExprEditorHeight	= 100;
	const JCoordinate kDerTableHeight	= 100;
	const JCoordinate kVarTableHeight	= 100;
	
	JArray<JCoordinate> heights(3);
	heights.AppendElement(kExprEditorHeight + kNameInputHeight);
	heights.AppendElement(kDerTableHeight);
	heights.AppendElement(kVarTableHeight);

	JIndex elasticIndex = 3;

	JArray<JCoordinate> minHeights(3);
	minHeights.AppendElement(80);
	minHeights.AppendElement(80);
	minHeights.AppendElement(80);
	
// begin JXLayout

    JXWindow* window = new JXWindow(this, 400,430, "");
    assert( window != NULL );
    SetWindow(window);

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
    assert( menuBar != NULL );

    itsPartition =
        new JXVertPartition(heights, elasticIndex, minHeights, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 380,345);
    assert( itsPartition != NULL );

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,400, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

    itsHelpButton =
        new JXTextButton("Help", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,400, 70,20);
    assert( itsHelpButton != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 310,400, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

// end JXLayout

	JXContainer* container	= itsPartition->GetCompartment(1);

// begin functionLayout

    itsNameInput =
        new JXInputField(container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 120,0, 260,20);
    assert( itsNameInput != NULL );

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(container,
                    JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 260,100);
    assert( scrollbarSet != NULL );

    JXStaticText* obj1 =
        new JXStaticText("Fit Name:", container,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 110,20);
    assert( obj1 != NULL );

    JXStaticText* obj2 =
        new JXStaticText("Fit function:", container,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 110,20);
    assert( obj2 != NULL );

// end functionLayout

	itsFnEditor	= 
		new JXExprEditor(itsVarList, menuBar,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsFnEditor != NULL);
	itsFnEditor->FitToEnclosure(kJTrue, kJTrue);

	container	= itsPartition->GetCompartment(2);

// begin derivativeLayout

    JXStaticText* derivativeLabel =
        new JXStaticText("Fit derivative:", container,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 100,20);
    assert( derivativeLabel != NULL );

    scrollbarSet =
        new JXScrollbarSet(container,
                    JXWidget::kHElastic, JXWidget::kVElastic, 120,0, 260,100);
    assert( scrollbarSet != NULL );

    JXStaticText* warningText =
        new JXStaticText("(This may be left\nblank. Glove will\nthen calculate the\nderivative.)", container,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 90,60);
    assert( warningText != NULL );
    warningText->SetFontSize(10);

// end derivativeLayout

	itsDerivativeEditor	= 
		new JXExprEditor(itsVarList, itsFnEditor,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsDerivativeEditor != NULL);
	itsDerivativeEditor->FitToEnclosure(kJTrue, kJTrue);

	container	= itsPartition->GetCompartment(3);

// begin variableLayout

    scrollbarSet =
        new JXScrollbarSet(container,
                    JXWidget::kHElastic, JXWidget::kVElastic, 120,0, 260,100);
    assert( scrollbarSet != NULL );

    JXStaticText* parmLabel =
        new JXStaticText("Parameters:", container,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 100,20);
    assert( parmLabel != NULL );

    itsNewButton =
        new JXTextButton("New", container,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,30, 65,20);
    assert( itsNewButton != NULL );
    itsNewButton->SetShortcuts("#N");

    itsDeleteButton =
        new JXTextButton("Delete", container,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,60, 65,20);
    assert( itsDeleteButton != NULL );

// end variableLayout

	itsVarTable	= 
		new GLVarTable(itsVarList, 
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsVarTable != NULL);
	itsVarTable->FitToEnclosure(kJTrue, kJTrue);

	ListenTo(itsNewButton);
	ListenTo(itsDeleteButton);
	itsDeleteButton->Deactivate();

	window->SetTitle("Non Linear Fit");
	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);
}


/******************************************************************************
 GetFunctionString

 ******************************************************************************/

JString
GLNonLinearFitDialog::GetFunctionString()
	const
{
	const JFunction* f = itsFnEditor->GetFunction();
	return f->Print();
}

/******************************************************************************
 GetDerivativeString

 ******************************************************************************/

JString
GLNonLinearFitDialog::GetDerivativeString()
	const
{
	const JFunction* f = itsDerivativeEditor->GetFunction();
	return f->Print();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GLNonLinearFitDialog::Receive
	(
	JBroadcaster* 	sender, 
	const Message&	message
	)
{
	if (sender == itsNewButton && message.Is(JXButton::kPushed))
		{
		itsVarTable->NewConstant();
		}
	else if (sender == itsDeleteButton && message.Is(JXButton::kPushed))
		{
		if (!OKToDeleteParm())
			{
			itsDeleteButton->Deactivate();
			}
		else
			{
			itsVarTable->RemoveSelectedConstant();
			}
		}
	else if (sender == itsDelButtonTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		if (OKToDeleteParm())
			{
			itsDeleteButton->Activate();
			}
		else
			{
			itsDeleteButton->Deactivate();
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
GLNonLinearFitDialog::OKToDeactivate()
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
	if (itsFnEditor->ContainsUIF())
		{
		JGetUserNotification()->ReportError("You must specify a fit function.");
		itsFnEditor->Focus();
		return kJFalse;
		}
	if (!itsDerivativeEditor->EndEditing())
		{
		return kJFalse;
		}
	if (!itsVarTable->EndEditing())
		{
		return kJFalse;
		}
	return kJTrue;
}

/******************************************************************************
 OKToDeleteParm (private)

 ******************************************************************************/

JBoolean
GLNonLinearFitDialog::OKToDeleteParm()
{
	JPoint cell;
	const JCoordinate kXOffset	= 1;
	if (itsVarTable->GetEditedCell(&cell) && itsVarList->OKToRemoveVariable(cell.y + kXOffset))
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 GetFitName (public)

 ******************************************************************************/

const JString&
GLNonLinearFitDialog::GetFitName()
	const
{
	return itsNameInput->GetText();
}
