/******************************************************************************
 CBEditCPPMacroDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditCPPMacroDialog.h"
#include "CBCPPMacroTable.h"
#include "cbGlobals.h"
#include "cbHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXColHeaderWidget.h>
#include <JXHelpManager.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditCPPMacroDialog::CBEditCPPMacroDialog
	(
	JXDirector*				supervisor,
	const CBCPreprocessor&	cpp
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	JPrefObject(CBGetPrefsManager(), kCBEditCPPMacroDialogID)
{
	BuildWindow(cpp);
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditCPPMacroDialog::~CBEditCPPMacroDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 UpdateMacros

	Returns kJTrue if anything changed.

 ******************************************************************************/

JBoolean
CBEditCPPMacroDialog::UpdateMacros
	(
	CBCPreprocessor* cpp
	)
	const
{
	return itsTable->UpdateMacros(cpp);
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditCPPMacroDialog::BuildWindow
	(
	const CBCPreprocessor& cpp
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,290, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 250,200);
	assert( scrollbarSet != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,260, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 260,260, 70,20);
	assert( okButton != NULL );

	JXTextButton* addMacroButton =
		new JXTextButton(JGetString("addMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,50, 60,20);
	assert( addMacroButton != NULL );
	addMacroButton->SetShortcuts(JGetString("addMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	JXTextButton* removeMacroButton =
		new JXTextButton(JGetString("removeMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,80, 60,20);
	assert( removeMacroButton != NULL );
	removeMacroButton->SetShortcuts(JGetString("removeMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	JXTextButton* loadMacroButton =
		new JXTextButton(JGetString("loadMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,130, 60,20);
	assert( loadMacroButton != NULL );
	loadMacroButton->SetShortcuts(JGetString("loadMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	JXTextButton* saveMacroButton =
		new JXTextButton(JGetString("saveMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,160, 60,20);
	assert( saveMacroButton != NULL );
	saveMacroButton->SetShortcuts(JGetString("saveMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 150,260, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	JXWidgetSet* colHeaderEncl =
		new JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 250,20);
	assert( colHeaderEncl != NULL );

// end JXLayout

	window->SetTitle("C Preprocessor");
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(kJFalse);
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsTable =
		new CBCPPMacroTable(cpp, addMacroButton, removeMacroButton,
							loadMacroButton, saveMacroButton,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTable != NULL );

	JXColHeaderWidget* colHeader =
		new JXColHeaderWidget(itsTable, scrollbarSet, colHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->TurnOnColResizing(20);
	itsTable->SetColTitles(colHeader);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CBEditCPPMacroDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBCTreeCPPHelpName);
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Check that the active object is willing to unfocus.

 ******************************************************************************/

JBoolean
CBEditCPPMacroDialog::OKToDeactivate()
{
	return JConvertToBoolean(
		Cancelled() ||
		(JXDialogDirector::OKToDeactivate() && itsTable->ContentsValid()) );
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBEditCPPMacroDialog::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		JXWindow* window = GetWindow();
		window->ReadGeometry(input);
		window->Deiconify();

		itsTable->ReadSetup(input);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBEditCPPMacroDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsTable->WriteSetup(output);
}
