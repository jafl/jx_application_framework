/******************************************************************************
 CBEditMacroDialog.cpp

	BASE CLASS = JXDialogDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBEditMacroDialog.h"
#include "CBMacroSetTable.h"
#include "CBCharActionTable.h"
#include "CBMacroTable.h"
#include "cbGlobals.h"
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
	JXDialogDirector(JXGetApplication(), true),
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

	auto* window = jnew JXWindow(this, 350,530, JString::empty);
	assert( window != nullptr );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 310,460);
	assert( itsPartition != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,500, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,500, 70,20);
	assert( okButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 145,500, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditMacroDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditMacroDialog"));
	SetButtons(okButton, cancelButton);
	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsHelpButton);

	// create action table

	JXContainer* compartment = itsPartition->GetCompartment(2);

// begin actionLayout

	const JRect actionLayout_Frame    = compartment->GetFrame();
	const JRect actionLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - actionLayout_Aperture.width(), 150 - actionLayout_Aperture.height());

	auto* newActionButton =
		jnew JXTextButton(JGetString("newActionButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newActionButton != nullptr );

	auto* removeActionButton =
		jnew JXTextButton(JGetString("removeActionButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeActionButton != nullptr );

	auto* actionScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( actionScrollbarSet != nullptr );

	auto* actionColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( actionColHeaderEncl != nullptr );

	auto* loadActionFileButton =
		jnew JXTextButton(JGetString("loadActionFileButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,100, 60,20);
	assert( loadActionFileButton != nullptr );

	auto* saveActionFileButton =
		jnew JXTextButton(JGetString("saveActionFileButton::CBEditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 60,20);
	assert( saveActionFileButton != nullptr );

	compartment->SetSize(actionLayout_Frame.width(), actionLayout_Frame.height());

// end actionLayout

	itsActionTable =
		jnew CBCharActionTable(this, newActionButton, removeActionButton,
							  loadActionFileButton, saveActionFileButton,
							  actionScrollbarSet, actionScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsActionTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsActionTable, actionScrollbarSet, actionColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != nullptr );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1Char::CBEditMacroDialog"));
	colHeader->SetColTitle(2, JGetString("Column2::CBEditMacroDialog"));
	colHeader->TurnOnColResizing(20);

	// create macro table

	compartment = itsPartition->GetCompartment(3);

// begin macroLayout

	const JRect macroLayout_Frame    = compartment->GetFrame();
	const JRect macroLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - macroLayout_Aperture.width(), 150 - macroLayout_Aperture.height());

	auto* newMacroButton =
		jnew JXTextButton(JGetString("newMacroButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newMacroButton != nullptr );

	auto* removeMacroButton =
		jnew JXTextButton(JGetString("removeMacroButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeMacroButton != nullptr );

	auto* macroScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( macroScrollbarSet != nullptr );

	auto* macroColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroColHeaderEncl != nullptr );

	auto* loadMacroFileButton =
		jnew JXTextButton(JGetString("loadMacroFileButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,100, 60,20);
	assert( loadMacroFileButton != nullptr );

	auto* saveMacroFileButton =
		jnew JXTextButton(JGetString("saveMacroFileButton::CBEditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 60,20);
	assert( saveMacroFileButton != nullptr );

	compartment->SetSize(macroLayout_Frame.width(), macroLayout_Frame.height());

// end macroLayout

	itsMacroTable =
		jnew CBMacroTable(this, newMacroButton, removeMacroButton,
						 loadMacroFileButton, saveMacroFileButton,
						 macroScrollbarSet, macroScrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsMacroTable != nullptr );

	colHeader =
		jnew JXColHeaderWidget(itsMacroTable, macroScrollbarSet, macroColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != nullptr );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1Macro::CBEditMacroDialog"));
	colHeader->SetColTitle(2, JGetString("Column2::CBEditMacroDialog"));
	colHeader->TurnOnColResizing(20);

	// create macro set table

	compartment = itsPartition->GetCompartment(1);

// begin macroSetLayout

	const JRect macroSetLayout_Frame    = compartment->GetFrame();
	const JRect macroSetLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - macroSetLayout_Aperture.width(), 150 - macroSetLayout_Aperture.height());

	auto* newMacroSetButton =
		jnew JXTextButton(JGetString("newMacroSetButton::CBEditMacroDialog::macroSetLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newMacroSetButton != nullptr );

	auto* removeMacroSetButton =
		jnew JXTextButton(JGetString("removeMacroSetButton::CBEditMacroDialog::macroSetLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeMacroSetButton != nullptr );

	auto* macroSetScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( macroSetScrollbarSet != nullptr );

	auto* macroSetColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroSetColHeaderEncl != nullptr );

	compartment->SetSize(macroSetLayout_Frame.width(), macroSetLayout_Frame.height());

// end macroSetLayout

	itsMacroSetTable =
		jnew CBMacroSetTable(macroList, initialSelection, firstUnusedID,
							itsActionTable, itsMacroTable,
							newMacroSetButton, removeMacroSetButton,
							macroSetScrollbarSet, macroSetScrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsMacroSetTable != nullptr );
	itsMacroSetTable->FitToEnclosure();

	colHeader =
		jnew JXColHeaderWidget(itsMacroSetTable, macroSetScrollbarSet, macroSetColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != nullptr );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1Set::CBEditMacroDialog"));
}

/******************************************************************************
 ContentsValid

	Check that the table contents are valid.

 ******************************************************************************/

bool
CBEditMacroDialog::ContentsValid()
	const
{
	return itsMacroSetTable->ContentsValid();
}

/******************************************************************************
 GetCurrentMacroSetName

 ******************************************************************************/

bool
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

bool
CBEditMacroDialog::OKToDeactivate()
{
	return Cancelled() ||
		(JXDialogDirector::OKToDeactivate() && ContentsValid());
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
		(JXGetHelpManager())->ShowSection("CBMacroHelp");
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
