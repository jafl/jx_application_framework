/******************************************************************************
 CBEditCommandsDialog.cpp

	BASE CLASS = JXDialogDirector, JPrefObject

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditCommandsDialog.h"
#include "CBCommandTable.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include "cbHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXVertPartition.h>
#include <JXScrollbarSet.h>
#include <JXColHeaderWidget.h>
#include <JXHelpManager.h>
#include <jStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

// string ID's

static const JCharacter* kWindowTitleID         = "WindowTitle::CBEditCommandsDialog";
static const JCharacter* kProjectTableCaptionID = "ProjectTableCaption::CBEditCommandsDialog";

/******************************************************************************
 Constructor

	thisCmdList can be NULL

 ******************************************************************************/

CBEditCommandsDialog::CBEditCommandsDialog
	(
	CBProjectDocument* projDoc
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue),
	JPrefObject(CBGetPrefsManager(), kCBEditCommandsDialogID),
	itsCmdMgr(projDoc == NULL ? NULL : projDoc->GetCommandManager())
{
	BuildWindow(projDoc);
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditCommandsDialog::~CBEditCommandsDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

	thisCmdList can be NULL

 ******************************************************************************/

void
CBEditCommandsDialog::BuildWindow
	(
	CBProjectDocument* projDoc
	)
{
	JArray<JCoordinate> heights(2);
	heights.AppendElement(180);
	heights.AppendElement(210);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendElement(100);
	minHeights.AppendElement(120);

	const JCoordinate kHeaderHeight = 20;

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 620,470, "");
	assert( window != NULL );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 580,395);
	assert( itsPartition != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,440, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 440,440, 70,20);
	assert( okButton != NULL );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 270,440, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditCommandsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString(kWindowTitleID));
	SetButtons(okButton, cancelButton);
	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsHelpButton);

	// commands available to all projects

	JXContainer* compartment;
	if (itsCmdMgr != NULL)
		{
		compartment = itsPartition->GetCompartment(1);
		}
	else
		{
		const JRect frame = itsPartition->GetFrame();
		compartment =
			jnew JXWidgetSet(window, JXWidget::kHElastic, JXWidget::kVElastic,
							frame.left, frame.top, frame.width(), frame.height());
		assert( compartment != NULL );
		itsPartition->Hide();
		}

// begin allProjectsLayout

	const JRect allProjectsLayout_Frame    = compartment->GetFrame();
	const JRect allProjectsLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(580 - allProjectsLayout_Aperture.width(), 140 - allProjectsLayout_Aperture.height());

	JXTextButton* allNewButton =
		jnew JXTextButton(JGetString("allNewButton::CBEditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,60, 60,20);
	assert( allNewButton != NULL );

	JXTextButton* allRemoveButton =
		jnew JXTextButton(JGetString("allRemoveButton::CBEditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,120, 60,20);
	assert( allRemoveButton != NULL );

	JXScrollbarSet* allScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 510,110);
	assert( allScrollbarSet != NULL );

	JXTextButton* allDuplicateButton =
		jnew JXTextButton(JGetString("allDuplicateButton::CBEditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,90, 60,20);
	assert( allDuplicateButton != NULL );

	JXStaticText* anyProjectTitle =
		jnew JXStaticText(JGetString("anyProjectTitle::CBEditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,3, 580,20);
	assert( anyProjectTitle != NULL );
	anyProjectTitle->SetToLabel();

	JXTextButton* allExportButton =
		jnew JXTextButton(JGetString("allExportButton::CBEditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,160, 60,20);
	assert( allExportButton != NULL );

	JXTextButton* allImportButton =
		jnew JXTextButton(JGetString("allImportButton::CBEditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,190, 60,20);
	assert( allImportButton != NULL );

	compartment->SetSize(allProjectsLayout_Frame.width(), allProjectsLayout_Frame.height());

// end allProjectsLayout

	if (itsCmdMgr == NULL)
		{
		allScrollbarSet->AdjustSize(0,
			(compartment->GetAperture()).bottom -
			(allScrollbarSet->GetFrame()).bottom);
		}

	const CBCommandManager::CmdList* allCmdList = (CBGetCommandManager())->GetCommandList();

	itsAllTable =
		jnew CBCommandTable(*allCmdList, allNewButton, allRemoveButton, allDuplicateButton,
						   allExportButton, allImportButton,
						   allScrollbarSet, allScrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsAllTable != NULL );
	itsAllTable->FitToEnclosure();
	itsAllTable->Move(0, kHeaderHeight);
	itsAllTable->AdjustSize(0, -kHeaderHeight);

	JXColHeaderWidget* colHeader =
		jnew JXColHeaderWidget(itsAllTable, allScrollbarSet,
							  allScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, 10,kHeaderHeight);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure(kJTrue, kJFalse);
	itsAllTable->SetColTitles(colHeader);
	colHeader->TurnOnColResizing(20);

	// commands available to currently selected project

	itsMakeDependCmd = NULL;
	itsThisTable     = NULL;

	if (itsCmdMgr != NULL)
		{
		compartment = itsPartition->GetCompartment(2);

// begin thisProjectLayout

	const JRect thisProjectLayout_Frame    = compartment->GetFrame();
	const JRect thisProjectLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(580 - thisProjectLayout_Aperture.width(), 170 - thisProjectLayout_Aperture.height());

	JXTextButton* thisNewButton =
		jnew JXTextButton(JGetString("thisNewButton::CBEditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,90, 60,20);
	assert( thisNewButton != NULL );

	JXTextButton* thisRemoveButton =
		jnew JXTextButton(JGetString("thisRemoveButton::CBEditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,150, 60,20);
	assert( thisRemoveButton != NULL );

	JXScrollbarSet* thisScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,70, 510,100);
	assert( thisScrollbarSet != NULL );

	itsMakeDependCmd =
		jnew JXInputField(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 260,40, 250,20);
	assert( itsMakeDependCmd != NULL );

	JXStaticText* updateMakefileLabel =
		jnew JXStaticText(JGetString("updateMakefileLabel::CBEditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,40, 260,20);
	assert( updateMakefileLabel != NULL );
	updateMakefileLabel->SetToLabel();

	JXTextButton* thisDuplicateButton =
		jnew JXTextButton(JGetString("thisDuplicateButton::CBEditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,120, 60,20);
	assert( thisDuplicateButton != NULL );

	JXStaticText* thisCaption =
		jnew JXStaticText(JGetString("thisCaption::CBEditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,10, 580,20);
	assert( thisCaption != NULL );
	thisCaption->SetToLabel();

	JXTextButton* thisExportButton =
		jnew JXTextButton(JGetString("thisExportButton::CBEditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,190, 60,20);
	assert( thisExportButton != NULL );

	JXTextButton* thisImportButton =
		jnew JXTextButton(JGetString("thisImportButton::CBEditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,220, 60,20);
	assert( thisImportButton != NULL );

	compartment->SetSize(thisProjectLayout_Frame.width(), thisProjectLayout_Frame.height());

// end thisProjectLayout

		const JCharacter* map[] =
			{
			"project", (projDoc->GetName()).GetCString()
			};
		thisCaption->SetText(JGetString(kProjectTableCaptionID, map, sizeof(map)));

		itsMakeDependCmd->SetText(itsCmdMgr->GetMakeDependCommand());
		itsMakeDependCmd->SetIsRequired();

		const CBCommandManager::CmdList* thisCmdList = itsCmdMgr->GetCommandList();

		itsThisTable =
			jnew CBCommandTable(*thisCmdList, thisNewButton, thisRemoveButton, thisDuplicateButton,
							   thisExportButton, thisImportButton,
							   thisScrollbarSet, thisScrollbarSet->GetScrollEnclosure(),
							   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
		assert( itsThisTable != NULL );
		itsThisTable->FitToEnclosure();
		itsThisTable->Move(0, kHeaderHeight);
		itsThisTable->AdjustSize(0, -kHeaderHeight);

		colHeader =
			jnew JXColHeaderWidget(itsThisTable, thisScrollbarSet,
								  thisScrollbarSet->GetScrollEnclosure(),
								  JXWidget::kHElastic, JXWidget::kFixedTop,
								  0,0, 10,kHeaderHeight);
		assert( colHeader != NULL );
		colHeader->FitToEnclosure(kJTrue, kJFalse);
		itsThisTable->SetColTitles(colHeader);
		colHeader->TurnOnColResizing(20);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditCommandsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsAllTable->GetCommandList((CBGetCommandManager())->GetCommandList());
			(CBGetCommandManager())->UpdateMenuIDs();

			if (itsCmdMgr != NULL)
				{
				assert( itsMakeDependCmd != NULL );
				itsCmdMgr->SetMakeDependCommand(itsMakeDependCmd->GetText());

				assert( itsThisTable != NULL );
				itsThisTable->GetCommandList(itsCmdMgr->GetCommandList());
				}

			(CBGetCommandManager())->UpdateAllCommandMenus();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBTasksHelpName);
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
CBEditCommandsDialog::ReadPrefs
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
	itsAllTable->ReadGeometry(input);

	if (itsThisTable != NULL)
		{
		itsThisTable->ReadGeometry(input);
		}
	else
		{
		JReadAll(input, &itsThisTableSetup);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBEditCommandsDialog::WritePrefs
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
	itsAllTable->WriteGeometry(output);

	if (itsThisTable != NULL)
		{
		output << ' ';
		itsThisTable->WriteGeometry(output);
		}
	else if (!itsThisTableSetup.IsEmpty())
		{
		if (itsThisTableSetup.GetFirstCharacter() != ' ')
			{
			output << ' ';
			}
		itsThisTableSetup.Print(output);
		}
	else	// use setup from itsAllTable
		{
		output << ' ';
		itsAllTable->WriteGeometry(output);
		}
}
