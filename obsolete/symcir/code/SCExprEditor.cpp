/******************************************************************************
 SCExprEditor.cpp

	Class to display expression being edited by user.

	BASE CLASS = JXExprEditor

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCExprEditor.h"
#include "SCCircuitDocument.h"
#include "SCCircuitVarList.h"
#include "SCNewPlotDialog.h"
#include "SCAddToPlotDialog.h"
#include "SCPlotDirector.h"
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXEPSPrinter.h>
#include <JVariableValue.h>
#include <JString.h>
#include <jAssert.h>

// Extra menu

const JCharacter* kExtraMenuTitleStr = "Extra";
const JCharacter* kExtraMenuStr =
	"Plot in new window %k Meta-P | Plot in existing window %k Meta-Shift-P"
	"%l| Substitute definition %k Meta-D";

static const JCharacter* kNewPlotFnCmdStr        = "Plot function in new window";
static const JCharacter* kNewPlotSelectionCmdStr = "Plot selection in new window";

static const JCharacter* kAddToPlotFnCmdStr        = "Plot function in existing window";
static const JCharacter* kAddToPlotSelectionCmdStr = "Plot selection in existing window";

enum
{
	kNewPlotCmd = 1,
	kAddToPlotCmd,
	kSubstDefCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SCExprEditor::SCExprEditor
	(
	SCCircuitDocument*		doc,
	JXMenuBar*				menuBar,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXExprEditor(doc->GetVarList(), menuBar, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
	SCExprEditorX(doc);

	itsExtraMenu = menuBar->AppendTextMenu(kExtraMenuTitleStr);
	itsExtraMenu->SetMenuItems(kExtraMenuStr);
	ListenTo(itsExtraMenu);
}

SCExprEditor::SCExprEditor
	(
	SCCircuitDocument*		doc,
	SCExprEditor*			menuProvider,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXExprEditor(doc->GetVarList(), menuProvider, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
	SCExprEditorX(doc);

	// Listen to the specified menus.  This is safe because
	// we know that they were created by another ExprWidget.

	itsExtraMenu = menuProvider->itsExtraMenu;
	ListenTo(itsExtraMenu);
}

// private

void
SCExprEditor::SCExprEditorX
	(
	SCCircuitDocument* doc
	)
{
	itsDocument        = doc;
	itsVarList         = itsDocument->GetVarList();
	itsNewPlotDialog   = NULL;
	itsAddToPlotDialog = NULL;

	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCExprEditor::~SCExprEditor()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCExprEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsExtraMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateExtraMenu();
		}
	else if (sender == itsExtraMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleExtraMenu(selection->GetIndex());
		}

	else if (sender == itsNewPlotDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			CreatePlot();
			}
		itsNewPlotDialog = NULL;
		}

	else if (sender == itsAddToPlotDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			AddToPlot();
			}
		itsAddToPlotDialog = NULL;
		}

	else
		{
		if (sender == this && message.Is(JExprEditor::kExprChanged))
			{
			itsDocument->DataModified();
			}

		JXExprEditor::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateExtraMenu (private)

 ******************************************************************************/

void
SCExprEditor::UpdateExtraMenu()
{
	itsExtraMenu->EnableItem(kNewPlotCmd);

	if (HasSelection())
		{
		itsExtraMenu->SetItemText(kNewPlotCmd, kNewPlotSelectionCmdStr);
		itsExtraMenu->SetItemText(kAddToPlotCmd, kAddToPlotSelectionCmdStr);

		const JVariableValue* f;
		if (SelectionIsVarFn(&f))
			{
			itsExtraMenu->EnableItem(kSubstDefCmd);
			}
		}
	else
		{
		itsExtraMenu->SetItemText(kNewPlotCmd, kNewPlotFnCmdStr);
		itsExtraMenu->SetItemText(kAddToPlotCmd, kAddToPlotFnCmdStr);
		}

	if (itsDocument->HasPlots())
		{
		itsExtraMenu->EnableItem(kAddToPlotCmd);
		}
}

/******************************************************************************
 HandleExtraMenu (private)

 ******************************************************************************/

void
SCExprEditor::HandleExtraMenu
	(
	const JIndex item
	)
{
	if (item == kNewPlotCmd)
		{
		AskCreatePlot();
		}
	else if (item == kAddToPlotCmd)
		{
		AskAddToPlot();
		}

	else if (item == kSubstDefCmd)
		{
		SubstituteDefinition();
		}
}

/******************************************************************************
 AskCreatePlot (private)

 ******************************************************************************/

void
SCExprEditor::AskCreatePlot()
{
	assert( itsNewPlotDialog == NULL );

	itsNewPlotDialog = new SCNewPlotDialog(itsDocument);
	assert( itsNewPlotDialog != NULL );
	ListenTo(itsNewPlotDialog);
	itsNewPlotDialog->BeginDialog();
}

/******************************************************************************
 CreatePlot (private)

 ******************************************************************************/

void
SCExprEditor::CreatePlot()
{
	assert( itsNewPlotDialog != NULL );

	const JFunction* f;
	if (!GetConstSelectedFunction(&f))
		{
		f = GetFunction();
		}

	JString curveName;
	JFloat fMin, fMax;
	itsNewPlotDialog->GetSettings(&curveName, &fMin, &fMax);

	SCPlotDirector* dir = new SCPlotDirector(itsDocument);
	assert( dir != NULL );
	dir->AddFunction(*f, curveName, fMin, fMax);
	dir->Activate();
}

/******************************************************************************
 AskAddToPlot (private)

 ******************************************************************************/

void
SCExprEditor::AskAddToPlot()
{
	assert( itsAddToPlotDialog == NULL );

	itsAddToPlotDialog = new SCAddToPlotDialog(itsDocument);
	assert( itsAddToPlotDialog != NULL );
	ListenTo(itsAddToPlotDialog);
	itsAddToPlotDialog->BeginDialog();
}

/******************************************************************************
 AddToPlot (private)

 ******************************************************************************/

void
SCExprEditor::AddToPlot()
{
	assert( itsAddToPlotDialog != NULL );

	const JFunction* f;
	if (!GetConstSelectedFunction(&f))
		{
		f = GetFunction();
		}

	JIndex plotIndex;
	JString curveName;
	JFloat fMin, fMax;
	itsAddToPlotDialog->GetSettings(&plotIndex, &curveName, &fMin, &fMax);

	SCPlotDirector* dir = itsDocument->GetPlot(plotIndex);
	dir->AddFunction(*f, curveName, fMin, fMax);
	dir->Activate();
}

/******************************************************************************
 SubstituteDefinition (private)

 ******************************************************************************/

void
SCExprEditor::SubstituteDefinition()
{
	const JVariableValue* varF;
	if (SelectionIsVarFn(&varF))
		{
		const JIndex varIndex = varF->GetVariableIndex();
		const JFunction* defnF;
		const JBoolean ok = itsVarList->GetFunction(varIndex, &defnF);
		assert( ok );

		const JString expr = defnF->Print();
		Paste(expr);
		}
}

/******************************************************************************
 SelectionIsVarFn (private)

	Returns kJTrue if the selection is a variable that happens to be a function.

 ******************************************************************************/

JBoolean
SCExprEditor::SelectionIsVarFn
	(
	const JVariableValue** varF
	)
	const
{
	const JFunction* f;
	if (GetConstSelectedFunction(&f) && f->GetType() == kJVariableValueType)
		{
		*varF = dynamic_cast<const JVariableValue*>(f);
		assert( varF != NULL );
		const JIndex varIndex = (**varF).GetVariableIndex();
		return itsVarList->IsFunction(varIndex);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReadState

 ******************************************************************************/

void
SCExprEditor::ReadState
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	JFunction* f = JFunction::StreamIn(input, itsVarList, kJTrue);
	SetFunction(itsVarList, f);

	JXEPSPrinter& p = GetEPSPrinter();
	if (vers < 2)
		{
		p.ReadEPSSetup(input);
		}
	else
		{
		p.ReadXEPSSetup(input);
		}
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
SCExprEditor::WriteState
	(
	std::ostream& output
	)
	const
{
	GetFunction()->StreamOut(output);

	output << ' ';
	(GetEPSPrinter()).WriteXEPSSetup(output);
}
