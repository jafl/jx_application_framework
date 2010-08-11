/******************************************************************************
 SCZinDirector.cpp

	BASE CLASS = SCDirectorBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCZinDirector.h"
#include "SCCircuitDocument.h"
#include "SCCircuit.h"
#include "SCCircuitVarList.h"
#include "SCExprEditor.h"
#include "SCExprEditorSet.h"
#include "SCNodeMenu.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <jXGlobals.h>

#include <jParseFunction.h>
#include <JFunction.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCZinDirector::SCZinDirector
	(
	SCCircuitDocument* supervisor
	)
	:
	SCDirectorBase(supervisor)
{
	BuildWindow(supervisor);
}

SCZinDirector::SCZinDirector
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuitDocument*	supervisor
	)
	:
	SCDirectorBase(input, vers, supervisor)
{
	BuildWindow(supervisor);
	(GetWindow())->ReadGeometry(input);

	JIndex i;
	input >> i;
	itsPosNode->SetNodeIndex(i);
	input >> i;
	itsNegNode->SetNodeIndex(i);

	itsResult->ReadState(input, vers);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCZinDirector::~SCZinDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SCZinDirector::BuildWindow
	(
	SCCircuitDocument* doc
	)
{
	const SCCircuit* circuit = doc->GetCircuit();

// begin JXLayout

    JXWindow* window = new JXWindow(this, 350,240, "");
    assert( window != NULL );

    itsEvalButton =
        new JXTextButton(JGetString("itsEvalButton::SCZinDirector::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 230,45, 80,20);
    assert( itsEvalButton != NULL );

    SCExprEditorSet* obj1_JXLayout =
        new SCExprEditorSet(doc, &itsResult, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,110, 350,130);
    assert( obj1_JXLayout != NULL );

    itsPosNode =
        new SCNodeMenu(circuit, "Positive node:", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,20, 160,30);
    assert( itsPosNode != NULL );

    itsNegNode =
        new SCNodeMenu(circuit, "Negative node:", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,60, 160,30);
    assert( itsNegNode != NULL );

// end JXLayout

	window->SetTitle("Input impedance");
	window->LockCurrentMinSize();

	itsPosNode->SetToPopupChoice(kJTrue);
	itsNegNode->SetToPopupChoice(kJTrue);

	ListenTo(itsEvalButton);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCZinDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEvalButton && message.Is(JXButton::kPushed))
		{
		EvaluateInputImpedance();
		}
	else
		{
		SCDirectorBase::Receive(sender, message);
		}
}

/******************************************************************************
 EvaluateInputImpedance (private)

 ******************************************************************************/

void
SCZinDirector::EvaluateInputImpedance()
{
	itsResult->ClearFunction();

	const SCCircuit* circuit = GetCircuit();
	if (!circuit->IsLinear())
		{
		(JGetUserNotification())->ReportError("The circuit is not linear.");
		return;
		}

	const JIndex posNode = itsPosNode->GetNodeIndex();
	const JIndex negNode = itsNegNode->GetNodeIndex();
	if (posNode == negNode)
		{
		(JGetUserNotification())->ReportError("The two nodes must be different.");
		return;
		}

	(GetCircuitDocument())->DataModified();

	const SCCircuitVarList* varList = GetVarList();

	JString Zin;
	JFunction* f;
	if (circuit->GetInputImpedance(posNode, negNode, &Zin) &&
		JParseFunction(Zin, varList, &f))
		{
		itsResult->SetFunction(varList, f);
		}
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
SCZinDirector::StreamOut
	(
	ostream& output
	)
	const
{
	output << ' ' << kZinType;

	output << ' ';
	(GetWindow())->WriteGeometry(output);

	output << ' ' << itsPosNode->GetNodeIndex();
	output << ' ' << itsNegNode->GetNodeIndex();

	output << ' ';
	itsResult->WriteState(output);

	output << ' ';
}
