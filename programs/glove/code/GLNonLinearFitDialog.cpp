/******************************************************************************
 GLNonLinearFitDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#include "GLNonLinearFitDialog.h"
#include "GLVarList.h"
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
#include <JXDeleteObjectTask.h>

#include <JFontManager.h>
#include <JFunction.h>
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
	JXDialogDirector(supervisor, true)
{
	itsVarList	= jnew GLVarList();
	assert(itsVarList != nullptr);

	itsVarList->AddVariable(JGetString("DefaultVarName::GLGlobal"), 0);

	BuildWindow();

	itsDelButtonTask	= jnew JXTimerTask(kDeleteButtonUpdateDelay);
	assert(itsDelButtonTask != nullptr);
	itsDelButtonTask->Start();
	ListenTo(itsDelButtonTask);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLNonLinearFitDialog::~GLNonLinearFitDialog()
{
	JXDeleteObjectTask<GLVarList>::Delete(itsVarList);
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

	auto* window = jnew JXWindow(this, 400,430, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 380,345);
	assert( itsPartition != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLNonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 20,400, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::GLNonLinearFitDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::GLNonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 165,400, 70,20);
	assert( itsHelpButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GLNonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 310,400, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLNonLinearFitDialog::shortcuts::JXLayout"));

// end JXLayout

	JXContainer* container	= itsPartition->GetCompartment(1);

// begin functionLayout

	const JRect functionLayout_Frame    = container->GetFrame();
	const JRect functionLayout_Aperture = container->GetAperture();
	container->AdjustSize(380 - functionLayout_Aperture.width(), 120 - functionLayout_Aperture.height());

	itsNameInput =
		jnew JXInputField(container,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,0, 260,20);
	assert( itsNameInput != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 260,100);
	assert( scrollbarSet != nullptr );

	auto* fitNameLabel =
		jnew JXStaticText(JGetString("fitNameLabel::GLNonLinearFitDialog::functionLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 110,20);
	assert( fitNameLabel != nullptr );
	fitNameLabel->SetToLabel();

	auto* fitFnLabel =
		jnew JXStaticText(JGetString("fitFnLabel::GLNonLinearFitDialog::functionLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 110,20);
	assert( fitFnLabel != nullptr );
	fitFnLabel->SetToLabel();

	container->SetSize(functionLayout_Frame.width(), functionLayout_Frame.height());

// end functionLayout

	itsFnEditor	= 
		jnew JXExprEditor(itsVarList, menuBar,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsFnEditor != nullptr);
	itsFnEditor->FitToEnclosure(true, true);

	container	= itsPartition->GetCompartment(2);

// begin derivativeLayout

	const JRect derivativeLayout_Frame    = container->GetFrame();
	const JRect derivativeLayout_Aperture = container->GetAperture();
	container->AdjustSize(380 - derivativeLayout_Aperture.width(), 100 - derivativeLayout_Aperture.height());

	auto* derivativeLabel =
		jnew JXStaticText(JGetString("derivativeLabel::GLNonLinearFitDialog::derivativeLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 100,20);
	assert( derivativeLabel != nullptr );
	derivativeLabel->SetToLabel();

	scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,0, 260,100);
	assert( scrollbarSet != nullptr );

	auto* warningText =
		jnew JXStaticText(JGetString("warningText::GLNonLinearFitDialog::derivativeLayout"), container,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 90,60);
	assert( warningText != nullptr );
	warningText->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	container->SetSize(derivativeLayout_Frame.width(), derivativeLayout_Frame.height());

// end derivativeLayout

	itsDerivativeEditor	= 
		jnew JXExprEditor(itsVarList, itsFnEditor,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsDerivativeEditor != nullptr);
	itsDerivativeEditor->FitToEnclosure(true, true);

	container	= itsPartition->GetCompartment(3);

// begin variableLayout

	const JRect variableLayout_Frame    = container->GetFrame();
	const JRect variableLayout_Aperture = container->GetAperture();
	container->AdjustSize(380 - variableLayout_Aperture.width(), 100 - variableLayout_Aperture.height());

	scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,0, 260,100);
	assert( scrollbarSet != nullptr );

	auto* parmLabel =
		jnew JXStaticText(JGetString("parmLabel::GLNonLinearFitDialog::variableLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 100,20);
	assert( parmLabel != nullptr );
	parmLabel->SetToLabel();

	itsNewButton =
		jnew JXTextButton(JGetString("itsNewButton::GLNonLinearFitDialog::variableLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,30, 65,20);
	assert( itsNewButton != nullptr );
	itsNewButton->SetShortcuts(JGetString("itsNewButton::GLNonLinearFitDialog::shortcuts::variableLayout"));

	itsDeleteButton =
		jnew JXTextButton(JGetString("itsDeleteButton::GLNonLinearFitDialog::variableLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,60, 65,20);
	assert( itsDeleteButton != nullptr );

	container->SetSize(variableLayout_Frame.width(), variableLayout_Frame.height());

// end variableLayout

	itsVarTable	= 
		jnew GLVarTable(itsVarList, 
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsVarTable != nullptr);
	itsVarTable->FitToEnclosure(true, true);

	ListenTo(itsNewButton);
	ListenTo(itsDeleteButton);
	itsDeleteButton->Deactivate();

	window->SetTitle(JGetString("WindowTitle::GLNonLinearFitDialog"));
	UseModalPlacement(false);
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

bool
GLNonLinearFitDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return false;
		}
	if (Cancelled())
		{
		return true;
		}
	JString name	= itsNameInput->GetText()->GetText();
	name.TrimWhitespace();
	if (name.IsEmpty())
		{
		JGetUserNotification()->ReportError(JGetString("MissingName::GLNonLinearFitDialog"));
		itsNameInput->Focus();
		return false;
		}
	if (itsFnEditor->ContainsUIF())
		{
		JGetUserNotification()->ReportError(JGetString("MissingFunction::GLNonLinearFitDialog"));
		itsFnEditor->Focus();
		return false;
		}
	if (!itsDerivativeEditor->EndEditing())
		{
		return false;
		}
	if (!itsVarTable->EndEditing())
		{
		return false;
		}
	return true;
}

/******************************************************************************
 OKToDeleteParm (private)

 ******************************************************************************/

bool
GLNonLinearFitDialog::OKToDeleteParm()
{
	JPoint cell;
	const JCoordinate kXOffset	= 1;
	if (itsVarTable->GetEditedCell(&cell) && itsVarList->OKToRemoveVariable(cell.y + kXOffset))
		{
		return true;
		}
	return false;
}

/******************************************************************************
 GetFitName (public)

 ******************************************************************************/

const JString&
GLNonLinearFitDialog::GetFitName()
	const
{
	return itsNameInput->GetText()->GetText();
}
