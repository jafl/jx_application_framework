/******************************************************************************
 CBEditCPPMacroDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBEditCPPMacroDialog.h"
#include "CBCPPMacroTable.h"
#include "cbGlobals.h"
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
	JXDialogDirector(supervisor, true),
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

	Returns true if anything changed.

 ******************************************************************************/

bool
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

	auto* window = jnew JXWindow(this, 370,290, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 250,200);
	assert( scrollbarSet != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,260, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 260,260, 70,20);
	assert( okButton != nullptr );

	auto* addMacroButton =
		jnew JXTextButton(JGetString("addMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,50, 60,20);
	assert( addMacroButton != nullptr );
	addMacroButton->SetShortcuts(JGetString("addMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	auto* removeMacroButton =
		jnew JXTextButton(JGetString("removeMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,80, 60,20);
	assert( removeMacroButton != nullptr );
	removeMacroButton->SetShortcuts(JGetString("removeMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	auto* loadMacroButton =
		jnew JXTextButton(JGetString("loadMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,130, 60,20);
	assert( loadMacroButton != nullptr );
	loadMacroButton->SetShortcuts(JGetString("loadMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	auto* saveMacroButton =
		jnew JXTextButton(JGetString("saveMacroButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,160, 60,20);
	assert( saveMacroButton != nullptr );
	saveMacroButton->SetShortcuts(JGetString("saveMacroButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 150,260, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditCPPMacroDialog::shortcuts::JXLayout"));

	auto* colHeaderEncl =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 250,20);
	assert( colHeaderEncl != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditCPPMacroDialog"));
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(false);
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsTable =
		jnew CBCPPMacroTable(cpp, addMacroButton, removeMacroButton,
							loadMacroButton, saveMacroButton,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, colHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != nullptr );
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
		(JXGetHelpManager())->ShowSection("CBCTreeHelp#CPP");
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

bool
CBEditCPPMacroDialog::OKToDeactivate()
{
	return Cancelled() ||
		(JXDialogDirector::OKToDeactivate() && itsTable->ContentsValid());
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBEditCPPMacroDialog::ReadPrefs
	(
	std::istream& input
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
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsTable->WriteSetup(output);
}
