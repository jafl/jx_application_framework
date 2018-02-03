/******************************************************************************
 SCXferFnDirector.cpp

	BASE CLASS = SCDirectorBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "SCXferFnDirector.h"
#include "SCCircuitDocument.h"
#include "SCCircuit.h"
#include "SCCircuitVarList.h"
#include "SCExprEditor.h"
#include "SCExprEditorSet.h"
#include "scGlobals.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextButton.h>
#include <JXVertPartition.h>

#include <jParseFunction.h>
#include <JFunction.h>
#include <JSymbolicMath.h>
#include <JString.h>
#include <jAssert.h>

const JCoordinate kInitExprHeight = 100;
const JCoordinate kMinExprHeight  = 50;

/******************************************************************************
 Constructor

 ******************************************************************************/

SCXferFnDirector::SCXferFnDirector
	(
	SCCircuitDocument* supervisor
	)
	:
	SCDirectorBase(supervisor)
{
	BuildWindow(supervisor);
}

SCXferFnDirector::SCXferFnDirector
	(
	std::istream&			input,
	const JFileVersion	vers,
	SCCircuitDocument*	supervisor
	)
	:
	SCDirectorBase(input, vers, supervisor)
{
	BuildWindow(supervisor);

	GetWindow()->ReadGeometry(input);
	itsPartition->ReadGeometry(input);

	itsXferFn->ReadState(input, vers);
	itsResult->ReadState(input, vers);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCXferFnDirector::~SCXferFnDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SCXferFnDirector::BuildWindow
	(
	SCCircuitDocument* doc
	)
{
	JArray<JCoordinate> heights(2);
	heights.AppendElement(kInitExprHeight);
	heights.AppendElement(kInitExprHeight);

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendElement(kMinExprHeight);
	minHeights.AppendElement(kMinExprHeight);

// begin JXLayout

	JXWindow* window = new JXWindow(this, 500,230, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 430,30);
	assert( menuBar != NULL );

	itsPartition =
		new JXVertPartition(heights, 0, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,205);
	assert( itsPartition != NULL );

	itsEvalButton =
		new JXTextButton(JGetString("itsEvalButton::SCXferFnDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,0, 70,30);
	assert( itsEvalButton != NULL );

// end JXLayout

	window->SetTitle("Transfer function");

	const JRect frame = itsPartition->GetFrame();
	window->SetMinSize(150, frame.top + itsPartition->GetMinTotalSize());

	ListenTo(itsEvalButton);

	SCExprEditorSet* exprSet =
		new SCExprEditorSet(doc, menuBar, &itsXferFn,
							itsPartition->GetCompartment(1),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsXferFn, &itsResult,
							itsPartition->GetCompartment(2),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCXferFnDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEvalButton && message.Is(JXButton::kPushed))
		{
		EvaluateTransferFunction();
		}
	else
		{
		SCDirectorBase::Receive(sender, message);
		}
}

/******************************************************************************
 EvaluateTransferFunction (private)

 ******************************************************************************/

void
SCXferFnDirector::EvaluateTransferFunction()
{
	if (!itsXferFn->EndEditing())
		{
		return;
		}
	else if (itsXferFn->ContainsUIF())
		{
		(JGetUserNotification())->ReportError("Please finish entering the function.");
		itsXferFn->Focus();
		return;
		}

	GetCircuitDocument()->DataModified();

	itsResult->ClearFunction();

	const SCCircuit* circuit = GetCircuit();
	if (!circuit->IsLinear())
		{
		(JGetUserNotification())->ReportError("The circuit is not linear.");
		return;
		}

	const SCCircuitVarList* varList = GetVarList();

	const JFunction* fn  = itsXferFn->GetFunction();
	const JString textFn = (SCGetSymbolicMath())->Print(*fn);
	JString result;
	JFunction* f;
	if (circuit->Evaluate(textFn, &result) &&
		JParseFunction(result, varList, &f))
		{
		itsResult->SetFunction(varList, f);
		}
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
SCXferFnDirector::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kXferFnType;

	output << ' ';
	GetWindow()->WriteGeometry(output);
	output << ' ';
	itsPartition->WriteGeometry(output);
	output << ' ';
	itsXferFn->WriteState(output);
	output << ' ';
	itsResult->WriteState(output);

	output << ' ';
}
