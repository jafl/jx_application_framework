/******************************************************************************
 CBEditMacroDialog.cpp

	BASE CLASS = JXDialogDirector, JPrefObject

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditMacroDialog.h"
#include "CBMacroSetTable.h"
#include "CBCharActionTable.h"
#include "CBMacroTable.h"
#include "cbGlobals.h"
#include "cbHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXVertPartition.h>
#include <JXScrollbarSet.h>
#include <JXColHeaderWidget.h>
#include <JXHelpManager.h>
#include <JString.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditMacroDialog::CBEditMacroDialog
	(
	JArray<CBPrefsManager::MacroSetInfo>*	macroList,
	const JIndex							initialSelection,
	const JIndex							firstUnusedID
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue),
	JPrefObject(CBGetPrefsManager(), kCBEditMacroDialogID)
{
	BuildWindow(macroList, initialSelection, firstUnusedID);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditMacroDialog::~CBEditMacroDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 GetMacroList

 ******************************************************************************/

JArray<CBPrefsManager::MacroSetInfo>*
CBEditMacroDialog::GetMacroList
	(
	JIndex* firstNewID,
	JIndex* lastNewID
	)
	const
{
	return itsMacroSetTable->GetMacroList(firstNewID, lastNewID);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBEditMacroDialog::BuildWindow
	(
	JArray<CBPrefsManager::MacroSetInfo>*	macroList,
	const JIndex							initialSelection,
	const JIndex							firstUnusedID
	)
{
	JArray<JCoordinate> heights(3);
	heights.AppendElement(150);
	heights.AppendElement(150);
	heights.AppendElement(150);

	const JIndex elasticIndex = 3;

	JArray<JCoordinate> minHeights(3);
	minHeights.AppendElement(60);
	minHeights.AppendElement(60);
	minHeights.AppendElement(60);

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 350,530, "");
	assert( window != NULL );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 310,460);
	assert( itsPartition != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,500, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,500, 70,20);
	assert( okButton != NULL );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 145,500, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditMacroDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Edit Actions & Macros");
	SetButtons(okButton, cancelButton);
	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsHelpButton);

	// create action table

	JXContainer* compartment = itsPartition->GetCompartment(2);

// begin actionLayout

	const JRect actionLayout_Frame    = compartment->GetFrame();
	const JRect actionLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - actionLayout_Aperture.width(), 150 - actionLayout_Aperture.height());
	dynamic_cast<JXWidgetSet*>(compartment)->SetExtraNeededSpace(310 - actionLayout_Aperture.width(), 150 - actionLayout_Aperture.height());

	JXTextButton* newActionButton =
		jnew JXTextButton(JGetString("newActionButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newActionButton != NULL );

	JXTextButton* removeActionButton =
		jnew JXTextButton(JGetString("removeActionButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeActionButton != NULL );

	JXScrollbarSet* actionScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( actionScrollbarSet != NULL );

	JXWidgetSet* actionColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( actionColHeaderEncl != NULL );

	JXTextButton* loadActionFileButton =
		jnew JXTextButton(JGetString("loadActionFileButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,100, 60,20);
	assert( loadActionFileButton != NULL );

	JXTextButton* saveActionFileButton =
		jnew JXTextButton(JGetString("saveActionFileButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 60,20);
	assert( saveActionFileButton != NULL );

	compartment->SetSize(actionLayout_Frame.width(), actionLayout_Frame.height());

// end actionLayout

	itsActionTable =
		jnew CBCharActionTable(this, newActionButton, removeActionButton,
							  loadActionFileButton, saveActionFileButton,
							  actionScrollbarSet, actionScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsActionTable != NULL );

	JXColHeaderWidget* colHeader =
		jnew JXColHeaderWidget(itsActionTable, actionScrollbarSet, actionColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, "Char");
	colHeader->SetColTitle(2, "Keystrokes");
	colHeader->TurnOnColResizing(20);

	// create macro table

	compartment = itsPartition->GetCompartment(3);

// begin macroLayout

	const JRect macroLayout_Frame    = compartment->GetFrame();
	const JRect macroLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - macroLayout_Aperture.width(), 150 - macroLayout_Aperture.height());
	dynamic_cast<JXWidgetSet*>(compartment)->SetExtraNeededSpace(310 - macroLayout_Aperture.width(), 150 - macroLayout_Aperture.height());

	JXTextButton* newMacroButton =
		jnew JXTextButton(JGetString("newMacroButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newMacroButton != NULL );

	JXTextButton* removeMacroButton =
		jnew JXTextButton(JGetString("removeMacroButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeMacroButton != NULL );

	JXScrollbarSet* macroScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( macroScrollbarSet != NULL );

	JXWidgetSet* macroColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroColHeaderEncl != NULL );

	JXTextButton* loadMacroFileButton =
		jnew JXTextButton(JGetString("loadMacroFileButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,100, 60,20);
	assert( loadMacroFileButton != NULL );

	JXTextButton* saveMacroFileButton =
		jnew JXTextButton(JGetString("saveMacroFileButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 60,20);
	assert( saveMacroFileButton != NULL );

	compartment->SetSize(macroLayout_Frame.width(), macroLayout_Frame.height());

// end macroLayout

	itsMacroTable =
		jnew CBMacroTable(this, newMacroButton, removeMacroButton,
						 loadMacroFileButton, saveMacroFileButton,
						 macroScrollbarSet, macroScrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsMacroTable != NULL );

	colHeader =
		jnew JXColHeaderWidget(itsMacroTable, macroScrollbarSet, macroColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, "Macro");
	colHeader->SetColTitle(2, "Keystrokes");
	colHeader->TurnOnColResizing(20);

	// create macro set table

	compartment = itsPartition->GetCompartment(1);

// begin macroSetLayout

	const JRect macroSetLayout_Frame    = compartment->GetFrame();
	const JRect macroSetLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - macroSetLayout_Aperture.width(), 150 - macroSetLayout_Aperture.height());
	dynamic_cast<JXWidgetSet*>(compartment)->SetExtraNeededSpace(310 - macroSetLayout_Aperture.width(), 150 - macroSetLayout_Aperture.height());

	JXTextButton* newMacroSetButton =
		jnew JXTextButton(JGetString("newMacroSetButton::CBEditMacroDialog::macroSetLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newMacroSetButton != NULL );

	JXTextButton* removeMacroSetButton =
		jnew JXTextButton(JGetString("removeMacroSetButton::CBEditMacroDialog::macroSetLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeMacroSetButton != NULL );

	JXScrollbarSet* macroSetScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( macroSetScrollbarSet != NULL );

	JXWidgetSet* macroSetColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroSetColHeaderEncl != NULL );

	compartment->SetSize(macroSetLayout_Frame.width(), macroSetLayout_Frame.height());

// end macroSetLayout

	itsMacroSetTable =
		jnew CBMacroSetTable(macroList, initialSelection, firstUnusedID,
							itsActionTable, itsMacroTable,
							newMacroSetButton, removeMacroSetButton,
							macroSetScrollbarSet, macroSetScrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsMacroSetTable != NULL );
	itsMacroSetTable->FitToEnclosure();

	colHeader =
		jnew JXColHeaderWidget(itsMacroSetTable, macroSetScrollbarSet, macroSetColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, "Name");
}

/******************************************************************************
 ContentsValid

	Check that the table contents are valid.

 ******************************************************************************/

JBoolean
CBEditMacroDialog::ContentsValid()
	const
{
	return itsMacroSetTable->ContentsValid();
}

/******************************************************************************
 GetCurrentMacroSetName

 ******************************************************************************/

JBoolean
CBEditMacroDialog::GetCurrentMacroSetName
	(
	JString* name
	)
	const
{
	return itsMacroSetTable->GetCurrentMacroSetName(name);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Check that the table contents are valid.

 ******************************************************************************/

JBoolean
CBEditMacroDialog::OKToDeactivate()
{
	return JConvertToBoolean(
		Cancelled() ||
		(JXDialogDirector::OKToDeactivate() && ContentsValid()) );
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditMacroDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBMacroHelpName);
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBEditMacroDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsPartition->ReadGeometry(input);

	JCoordinate w;
	input >> w;
	itsActionTable->SetColWidth(CBCharActionTable::kMacroColumn, w);
	input >> w;
	itsMacroTable->SetColWidth(CBMacroTable::kMacroColumn, w);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBEditMacroDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsPartition->WriteGeometry(output);

	output << ' ' << itsActionTable->GetColWidth(CBCharActionTable::kMacroColumn);
	output << ' ' << itsMacroTable->GetColWidth(CBMacroTable::kMacroColumn);
}
