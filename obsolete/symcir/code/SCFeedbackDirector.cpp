/******************************************************************************
 SCFeedbackDirector.cpp

	BASE CLASS = SCDirectorBase

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCFeedbackDirector.h"
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

static const JCharacter* kBuildH0FormItemStr   = "Build H0 form";
static const JCharacter* kBuildHinfFormItemStr = "Build Hinf form";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCFeedbackDirector::SCFeedbackDirector
	(
	SCCircuitDocument* supervisor
	)
	:
	SCDirectorBase(supervisor)
{
	BuildWindow(supervisor);
}

SCFeedbackDirector::SCFeedbackDirector
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
	itsHPartition->ReadGeometry(input);
	itsTPartition->ReadGeometry(input);

	JIndex i;
	input >> i;
	itsInputSource->SetCompIndex(i);
	input >> i;
	itsDepSource->SetCompIndex(i);

	itsOutputFn->ReadState(input, vers);
	itsH0->ReadState(input, vers);
	itsHinf->ReadState(input, vers);
	itsT->ReadState(input, vers);
	itsTn->ReadState(input, vers);
	itsScratchFn->ReadState(input, vers);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCFeedbackDirector::~SCFeedbackDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SCFeedbackDirector::BuildWindow
	(
	SCCircuitDocument* doc
	)
{
	const SCCircuit* circuit = doc->GetCircuit();

	JArray<JCoordinate> heights(4);
	heights.AppendElement(kInitExprHeight);
	heights.AppendElement(kInitExprHeight);
	heights.AppendElement(kInitExprHeight);
	heights.AppendElement(kInitExprHeight);

	JArray<JCoordinate> minHeights(4);
	minHeights.AppendElement(kMinExprHeight);
	minHeights.AppendElement(kMinExprHeight);
	minHeights.AppendElement(kMinExprHeight);
	minHeights.AppendElement(kMinExprHeight);

// begin JXLayout

	JXWindow* window = new JXWindow(this, 360,520, "");
	assert( window != NULL );

	itsMainPartition =
		new JXVertPartition(heights, 0, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,110, 365,415);
	assert( itsMainPartition != NULL );

	itsInputSource =
		new SCComponentMenu(circuit, SCACSourceFilter, "Input source:", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 150,30);
	assert( itsInputSource != NULL );

	itsDepSource =
		new SCComponentMenu(circuit, SCDepSourceFilter, "Dependent source:", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 150,30);
	assert( itsDepSource != NULL );

	itsEvalButton =
		new JXTextButton(JGetString("itsEvalButton::SCFeedbackDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,20, 80,20);
	assert( itsEvalButton != NULL );

	JXStaticText* layoutMessage =
		new JXStaticText(JGetString("layoutMessage::SCFeedbackDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,50, 110,60);
	assert( layoutMessage != NULL );

// end JXLayout

	const JCoordinate kMinWindowWidth = window->GetBoundsWidth();

	window->SetTitle("Feedback Theorem");
	window->SetMinSize(kMinWindowWidth, window->GetBoundsHeight());

	layoutMessage->SetText(
		"Output fn\nH0  |  Hinf\n T   |  Tn\nScratch area");

	itsInputSource->SetToPopupChoice(kJTrue);
	itsDepSource->SetToPopupChoice(kJTrue);

	ListenTo(itsEvalButton);

	// create sub-partitions

	const JCoordinate w =
		(itsMainPartition->GetBoundsWidth() - JPartition::kDragRegionSize)/2;
	JArray<JCoordinate> widths(2);
	widths.AppendElement(w);
	widths.AppendElement(w);

	const JCoordinate wMin = (kMinWindowWidth - JPartition::kDragRegionSize)/2;
	JArray<JCoordinate> minWidths(2);
	minWidths.AppendElement(wMin);
	minWidths.AppendElement(wMin);

	JXContainer* encl = itsMainPartition->GetCompartment(2);
	itsHPartition =
		new JXHorizPartition(widths, 0, minWidths, encl,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, encl->GetBoundsWidth(), encl->GetBoundsHeight());
	assert( itsHPartition != NULL );

	encl = itsMainPartition->GetCompartment(3);
	itsTPartition =
		new JXHorizPartition(widths, 0, minWidths, encl,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, encl->GetBoundsWidth(), encl->GetBoundsHeight());
	assert( itsTPartition != NULL );

	// create expressions

	SCExprEditorSet* exprSet =
		new SCExprEditorSet(doc, &itsOutputFn,
							itsMainPartition->GetCompartment(1),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsOutputFn, &itsH0,
							itsHPartition->GetCompartment(1),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsOutputFn, &itsHinf,
							itsHPartition->GetCompartment(2),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsOutputFn, &itsT,
							itsTPartition->GetCompartment(1),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsOutputFn, &itsTn,
							itsTPartition->GetCompartment(2),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	exprSet =
		new SCExprEditorSet(doc, itsOutputFn, &itsScratchFn,
							itsMainPartition->GetCompartment(4),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	assert( exprSet != NULL );
	exprSet->FitToEnclosure();

	// add our menu items

	JXTextMenu* menu = itsOutputFn->GetExtraMenu();

	menu->AppendItem(kBuildH0FormItemStr);
	itsBuildH0Index = menu->GetItemCount();

	menu->AppendItem(kBuildHinfFormItemStr);
	itsBuildHinfIndex = menu->GetItemCount();

	menu->ShowSeparatorAfter(itsBuildH0Index-1);
	ListenTo(menu);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCFeedbackDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* menu = itsOutputFn->GetExtraMenu();

	if (sender == itsEvalButton && message.Is(JXButton::kPushed))
		{
		EvaluateFeedbackParameters();
		}

	else if (sender == menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateExtraMenu();
		}
	else if (sender == menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleExtraMenu(selection->GetIndex());
		}

	else
		{
		SCDirectorBase::Receive(sender, message);
		}
}

/******************************************************************************
 EvaluateFeedbackParameters (private)

 ******************************************************************************/

void
SCFeedbackDirector::EvaluateFeedbackParameters()
{
	if (!itsOutputFn->EndEditing())
		{
		return;
		}
	else if (itsOutputFn->ContainsUIF())
		{
		(JGetUserNotification())->ReportError("Please finish entering the output function.");
		itsOutputFn->Focus();
		return;
		}

	GetCircuitDocument()->DataModified();

	itsH0->ClearFunction();
	itsHinf->ClearFunction();
	itsT->ClearFunction();
	itsTn->ClearFunction();
	itsScratchFn->ClearFunction();

	const SCCircuit* circuit = GetCircuit();
	if (!circuit->IsLinear())
		{
		(JGetUserNotification())->ReportError("The circuit is not linear.");
		return;
		}

	JIndex indepSourceIndex;
	if (!itsInputSource->GetCompIndex(&indepSourceIndex))
		{
		(JGetUserNotification())->ReportError(
			"There are no independent sources in this circuit.");
		return;
		}

	JIndex depSourceIndex;
	if (!itsDepSource->GetCompIndex(&depSourceIndex))
		{
		(JGetUserNotification())->ReportError(
			"There are no dependent sources in this circuit.");
		return;
		}

	const SCCircuitVarList* varList = GetVarList();

	const JFunction* fn  = itsOutputFn->GetFunction();
	const JString textFn = (SCGetSymbolicMath())->Print(*fn);
	JString H0, Hinf, T, Tn;
	if (!circuit->GetFeedbackParameters(indepSourceIndex, textFn, depSourceIndex,
										&H0, &Hinf, &T, &Tn))
		{
		return;
		}

	JFunction* f;
	if (JParseFunction(H0, varList, &f))
		{
		itsH0->SetFunction(varList, f);
		}
	if (JParseFunction(Hinf, varList, &f))
		{
		itsHinf->SetFunction(varList, f);
		}
	if (JParseFunction(T, varList, &f))
		{
		itsT->SetFunction(varList, f);
		}
	if (JParseFunction(Tn, varList, &f))
		{
		itsTn->SetFunction(varList, f);
		}
}

/******************************************************************************
 UpdateExtraMenu (private)

 ******************************************************************************/

void
SCFeedbackDirector::UpdateExtraMenu()
{
	JXTextMenu* menu = itsOutputFn->GetExtraMenu();
	menu->EnableItem(itsBuildH0Index);
	menu->EnableItem(itsBuildHinfIndex);
}

/******************************************************************************
 HandleExtraMenu (private)

 ******************************************************************************/

void
SCFeedbackDirector::HandleExtraMenu
	(
	const JIndex item
	)
{
	if (item != itsBuildH0Index && item != itsBuildHinfIndex)
		{
		return;
		}

	JString s;
	if (item == itsBuildH0Index)
		{
		s  = "(";
		s += (itsH0->GetFunction())->Print();
		s += ") * ((1 + (";
		s += (itsTn->GetFunction())->Print();
		s += "))/(1 + (";
		s += (itsT->GetFunction())->Print();
		s += ")))";
		}
	else
		{
		assert( item == itsBuildHinfIndex );

		s  = "(";
		s += (itsHinf->GetFunction())->Print();
		s += ") * ((1 + (1/(";
		s += (itsTn->GetFunction())->Print();
		s += ")))/(1 + (1/(";
		s += (itsT->GetFunction())->Print();
		s += "))))";
		}

	const SCCircuitVarList* varList = GetVarList();
	JFunction* f;
	const JBoolean ok = JParseFunction(s, varList, &f, kJTrue);
	assert( ok );
	itsScratchFn->SetFunction(varList, f);
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
SCFeedbackDirector::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kFeedbackType;

	output << ' ';
	GetWindow()->WriteGeometry(output);
	output << ' ';
	itsMainPartition->WriteGeometry(output);
	output << ' ';
	itsHPartition->WriteGeometry(output);
	output << ' ';
	itsTPartition->WriteGeometry(output);

	JIndex i;
	itsInputSource->GetCompIndex(&i);
	output << ' ' << i;
	itsDepSource->GetCompIndex(&i);
	output << ' ' << i;

	output << ' ';
	itsOutputFn->WriteState(output);
	output << ' ';
	itsH0->WriteState(output);
	output << ' ';
	itsHinf->WriteState(output);
	output << ' ';
	itsT->WriteState(output);
	output << ' ';
	itsTn->WriteState(output);
	output << ' ';
	itsScratchFn->WriteState(output);

	output << ' ';
}
