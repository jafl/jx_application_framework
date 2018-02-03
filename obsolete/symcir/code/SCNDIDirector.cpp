/******************************************************************************
 SCNDIDirector.cpp

	BASE CLASS = SCDirectorBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "SCNDIDirector.h"
#include "SCCircuitDocument.h"
#include "SCCircuit.h"
#include "SCCircuitVarList.h"
#include "SCExprEditor.h"
#include "SCExprEditorSet.h"
#include "SCComponentMenu.h"
#include "scGlobals.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXHorizPartition.h>
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

SCNDIDirector::SCNDIDirector
	(
	SCCircuitDocument* supervisor
	)
	:
	SCDirectorBase(supervisor)
{
	BuildWindow(supervisor);
}

SCNDIDirector::SCNDIDirector
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
	itsMainPartition->ReadGeometry(input);
	itsInputPartition->ReadGeometry(input);

	JIndex i;
	input >> i;
	itsSource1->SetCompIndex(i);
	input >> i;
	itsSource2->SetCompIndex(i);

	itsXferFn->ReadState(input, vers);
	itsNullFn->ReadState(input, vers);
	itsResult->ReadState(input, vers);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCNDIDirector::~SCNDIDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SCNDIDirector::BuildWindow
	(
	SCCircuitDocument* doc
	)
{
	const SCCircuit* circuit = doc->GetCircuit();

	JArray<JCoordinate> heights(2);
	heights.AppendElement(kInitExprHeight);
	heights.AppendElement(kInitExprHeight);

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendElement(kMinExprHeight);
	minHeights.AppendElement(kMinExprHeight);

// begin JXLayout

	JXWindow* window = new JXWindow(this, 360,340, "");
	assert( window != NULL );

	itsMainPartition =
		new JXVertPartition(heights, 0, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,140, 365,205);
	assert( itsMainPartition != NULL );

	itsSource1 =
		new SCComponentMenu(circuit, SCACSourceFilter, "First source:", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 150,30);
	assert( itsSource1 != NULL );

	itsSource2 =
		new SCComponentMenu(circuit, SCACSourceFilter, "Second source:", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 150,30);
	assert( itsSource2 != NULL );

	itsEvalButton =
		new JXTextButton(JGetString("itsEvalButton::SCNDIDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,20, 80,20);
	assert( itsEvalButton != NULL );

	JXStaticText* layoutMessage =
		new JXStaticText(JGetString("layoutMessage::SCNDIDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,50, 120,40);
	assert( layoutMessage != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,110, 365,30);
	assert( menuBar != NULL );

// end JXLayout

	const JCoordinate kMinWindowWidth = window->GetBoundsWidth();

	window->SetTitle("Null Double Injection");
	window->SetMinSize(kMinWindowWidth, window->GetBoundsHeight());

	layoutMessage->SetText(
		"Output fn | Nulled fn\n          Result");

	itsSource1->SetToPopupChoice(kJTrue);
	itsSource2->SetToPopupChoice(kJTrue);

	ListenTo(itsEvalButton);

	// create sub-partition

	const JCoordinate w =
		(itsMainPartition->GetBoundsWidth() - JPartition::kDragRegionSize)/2;
	JArray<JCoordinate> widths(2);
	widths.AppendElement(w);
	widths.AppendElement(w);

	const JCoordinate wMin = (kMinWindowWidth - JPartition::kDragRegionSize)/2;
	JArray<JCoordinate> minWidths(2);
	minWidths.AppendElement(wMin);
	minWidths.AppendElement(wMin);

	JXContainer* encl = itsMainPartition->GetCompartment(1);
	itsInputPartition =
		new JXHorizPartition(widths, 0, minWidths, encl,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, encl->GetBoundsWidth(), encl->GetBoundsHeight());
	assert( itsInputPartition != NULL );

	// create expressions

	SCExprEditorSet* exprSet =
		new SCExprEditorSet(doc, menuBar, &itsXferFn,
							itsInputPartition->GetCompartment(1),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsXferFn, &itsNullFn,
							itsInputPartition->GetCompartment(2),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsXferFn, &itsResult,
							itsMainPartition->GetCompartment(2),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCNDIDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEvalButton && message.Is(JXButton::kPushed))
		{
		EvaluateNDITransferFunction();
		}

	else
		{
		SCDirectorBase::Receive(sender, message);
		}
}

/******************************************************************************
 EvaluateNDITransferFunction (private)

 ******************************************************************************/

void
SCNDIDirector::EvaluateNDITransferFunction()
{
	if (!itsXferFn->EndEditing() || !itsNullFn->EndEditing())
		{
		return;
		}
	else if (itsXferFn->ContainsUIF())
		{
		(JGetUserNotification())->ReportError("Please finish entering the output function.");
		itsXferFn->Focus();
		return;
		}
	else if (itsNullFn->ContainsUIF())
		{
		(JGetUserNotification())->ReportError("Please finish entering the function to be nulled.");
		itsNullFn->Focus();
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

	JIndex source1Index, source2Index;
	if (!itsSource1->GetCompIndex(&source1Index) ||
		!itsSource2->GetCompIndex(&source2Index))
		{
		(JGetUserNotification())->ReportError(
			"There are no independent sources in this circuit.");
		return;
		}
	else if (source1Index == source2Index)
		{
		(JGetUserNotification())->ReportError("The two sources must be different.");
		return;
		}

	const SCCircuitVarList* varList = GetVarList();

	JArray<JIndex> inputList(2);
	inputList.AppendElement(source1Index);
	inputList.AppendElement(source2Index);

	const JFunction* nullFn = itsNullFn->GetFunction();
	JString textNullFn      = (SCGetSymbolicMath())->Print(*nullFn);

	JPtrArray<JString> nullList(JPtrArrayT::kForgetAll, 1);
	nullList.Append(&textNullFn);

	const JFunction* fn  = itsXferFn->GetFunction();
	const JString textFn = (SCGetSymbolicMath())->Print(*fn);
	JString result;
	JFunction* f;
	if (circuit->Evaluate(textFn, inputList, nullList, &result) &&
		JParseFunction(result, varList, &f))
		{
		itsResult->SetFunction(varList, f);
		}
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
SCNDIDirector::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kNDIType;

	output << ' ';
	GetWindow()->WriteGeometry(output);
	output << ' ';
	itsMainPartition->WriteGeometry(output);
	output << ' ';
	itsInputPartition->WriteGeometry(output);

	JIndex i;
	itsSource1->GetCompIndex(&i);
	output << ' ' << i;
	itsSource2->GetCompIndex(&i);
	output << ' ' << i;

	output << ' ';
	itsXferFn->WriteState(output);
	output << ' ';
	itsNullFn->WriteState(output);
	output << ' ';
	itsResult->WriteState(output);

	output << ' ';
}
