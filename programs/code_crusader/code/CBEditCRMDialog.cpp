/******************************************************************************
 CBEditCRMDialog.cpp

	BASE CLASS = JXDialogDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBEditCRMDialog.h"
#include "CBCRMRuleListTable.h"
#include "CBCRMRuleTable.h"
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

CBEditCRMDialog::CBEditCRMDialog
	(
	JArray<CBPrefsManager::CRMRuleListInfo>*	crmList,
	const JIndex								initialSelection,
	const JIndex								firstUnusedID
	)
	:
	JXDialogDirector(JXGetApplication(), true),
	JPrefObject(CBGetPrefsManager(), kCBEditCRMDialogID)
{
	BuildWindow(crmList, initialSelection, firstUnusedID);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditCRMDialog::~CBEditCRMDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 GetCRMRuleLists

 ******************************************************************************/

JArray<CBPrefsManager::CRMRuleListInfo>*
CBEditCRMDialog::GetCRMRuleLists
	(
	JIndex* firstNewID,
	JIndex* lastNewID
	)
	const
{
	return itsCRMTable->GetCRMRuleLists(firstNewID, lastNewID);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBEditCRMDialog::BuildWindow
	(
	JArray<CBPrefsManager::CRMRuleListInfo>*	crmList,
	const JIndex								initialSelection,
	const JIndex								firstUnusedID
	)
{
	JArray<JCoordinate> heights(2);
	heights.AppendElement(150);
	heights.AppendElement(150);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendElement(60);
	minHeights.AppendElement(60);

// begin JXLayout

	auto* window = jnew JXWindow(this, 540,370, JString::empty);
	assert( window != nullptr );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 500,305);
	assert( itsPartition != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,340, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 400,340, 70,20);
	assert( okButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 235,340, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditCRMDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditCRMDialog"));
	SetButtons(okButton, cancelButton);
	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsHelpButton);

	// create rule table

	JXContainer* compartment = itsPartition->GetCompartment(2);

// begin ruleLayout

	const JRect ruleLayout_Frame    = compartment->GetFrame();
	const JRect ruleLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(500 - ruleLayout_Aperture.width(), 150 - ruleLayout_Aperture.height());

	auto* newRuleButton =
		jnew JXTextButton(JGetString("newRuleButton::CBEditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,30, 60,20);
	assert( newRuleButton != nullptr );

	auto* removeRuleButton =
		jnew JXTextButton(JGetString("removeRuleButton::CBEditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 60,20);
	assert( removeRuleButton != nullptr );

	auto* ruleScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 430,150);
	assert( ruleScrollbarSet != nullptr );

	auto* loadRuleFileButton =
		jnew JXTextButton(JGetString("loadRuleFileButton::CBEditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,100, 60,20);
	assert( loadRuleFileButton != nullptr );

	auto* saveRuleFileButton =
		jnew JXTextButton(JGetString("saveRuleFileButton::CBEditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,130, 60,20);
	assert( saveRuleFileButton != nullptr );

	compartment->SetSize(ruleLayout_Frame.width(), ruleLayout_Frame.height());

// end ruleLayout

	JXWidgetSet* encl                  = ruleScrollbarSet->GetScrollEnclosure();
	const JCoordinate kColHeaderHeight = 20;
	const JCoordinate w                = encl->GetApertureWidth();

	itsRuleTable =
		jnew CBCRMRuleTable(this, newRuleButton, removeRuleButton,
						   loadRuleFileButton, saveRuleFileButton,
						   ruleScrollbarSet, encl,
						   JXWidget::kHElastic, JXWidget::kVElastic,
						   0, kColHeaderHeight,
						   w, encl->GetApertureHeight() - kColHeaderHeight);
	assert( itsRuleTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsRuleTable, ruleScrollbarSet, encl,
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, w, kColHeaderHeight);
	assert( colHeader != nullptr );
	colHeader->TurnOnColResizing(20);
	itsRuleTable->SetColTitles(colHeader);

	// create CRM table

	compartment = itsPartition->GetCompartment(1);

// begin crmLayout

	const JRect crmLayout_Frame    = compartment->GetFrame();
	const JRect crmLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(500 - crmLayout_Aperture.width(), 150 - crmLayout_Aperture.height());

	auto* newCRMButton =
		jnew JXTextButton(JGetString("newCRMButton::CBEditCRMDialog::crmLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,30, 60,20);
	assert( newCRMButton != nullptr );

	auto* removeCRMButton =
		jnew JXTextButton(JGetString("removeCRMButton::CBEditCRMDialog::crmLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 60,20);
	assert( removeCRMButton != nullptr );

	auto* crmScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 430,130);
	assert( crmScrollbarSet != nullptr );

	auto* crmColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 430,20);
	assert( crmColHeaderEncl != nullptr );

	compartment->SetSize(crmLayout_Frame.width(), crmLayout_Frame.height());

// end crmLayout

	itsCRMTable =
		jnew CBCRMRuleListTable(crmList, initialSelection, firstUnusedID, itsRuleTable,
							   newCRMButton, removeCRMButton,
							   crmScrollbarSet, crmScrollbarSet->GetScrollEnclosure(),
							   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCRMTable != nullptr );
	itsCRMTable->FitToEnclosure();

	colHeader =
		jnew JXColHeaderWidget(itsCRMTable, crmScrollbarSet, crmColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != nullptr );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1::CBEditCRMDialog"));
}

/******************************************************************************
 GetCurrentCRMRuleSetName

 ******************************************************************************/

bool
CBEditCRMDialog::GetCurrentCRMRuleSetName
	(
	JString* name
	)
	const
{
	return itsCRMTable->GetCurrentCRMRuleSetName(name);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditCRMDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("CBCRMHelp");
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
CBEditCRMDialog::ReadPrefs
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
	itsRuleTable->ReadGeometry(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBEditCRMDialog::WritePrefs
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

	output << ' ';
	itsRuleTable->WriteGeometry(output);
}
