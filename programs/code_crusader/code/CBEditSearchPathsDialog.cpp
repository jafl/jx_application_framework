/******************************************************************************
 CBEditSearchPathsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBEditSearchPathsDialog.h"
#include "CBPathTable.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JString.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

// string ID's

static const JCharacter* kInstructionsID = "Instructions::CBEditSearchPathsDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditSearchPathsDialog::CBEditSearchPathsDialog
	(
	JXDirector*			supervisor,
	const CBDirList&	dirList,
	CBRelPathCSF*		csf
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	JPrefObject(CBGetPrefsManager(), kCBEditSearchPathsDialogID)
{
	BuildWindow(dirList, csf);
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditSearchPathsDialog::~CBEditSearchPathsDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 AddDirectories

 ******************************************************************************/

void
CBEditSearchPathsDialog::AddDirectories
	(
	const JPtrArray<JString>& list
	)
{
	itsTable->AddDirectories(list);
}

/******************************************************************************
 GetPathList

 ******************************************************************************/

void
CBEditSearchPathsDialog::GetPathList
	(
	CBDirList* pathList
	)
	const
{
	itsTable->GetPathList(pathList);
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditSearchPathsDialog::BuildWindow
	(
	const CBDirList&	dirList,
	CBRelPathCSF*		csf
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,370, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,100, 240,220);
	assert( scrollbarSet != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,340, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 230,340, 70,20);
	assert( okButton != NULL );

	JXStaticText* instrText =
		new JXStaticText(JGetString("instrText::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 330,70);
	assert( instrText != NULL );

	JXTextButton* addPathButton =
		new JXTextButton(JGetString("addPathButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,110, 70,20);
	assert( addPathButton != NULL );
	addPathButton->SetShortcuts(JGetString("addPathButton::CBEditSearchPathsDialog::shortcuts::JXLayout"));

	JXTextButton* removePathButton =
		new JXTextButton(JGetString("removePathButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,140, 70,20);
	assert( removePathButton != NULL );
	removePathButton->SetShortcuts(JGetString("removePathButton::CBEditSearchPathsDialog::shortcuts::JXLayout"));

	JXTextButton* choosePathButton =
		new JXTextButton(JGetString("choosePathButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,190, 70,20);
	assert( choosePathButton != NULL );
	choosePathButton->SetShortcuts(JGetString("choosePathButton::CBEditSearchPathsDialog::shortcuts::JXLayout"));

// end JXLayout

	instrText->SetText(JGetString(kInstructionsID));
	window->AdjustSize(0, instrText->GetBoundsHeight() - instrText->GetFrameHeight());
	instrText->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);
	scrollbarSet->SetSizing(JXWidget::kHElastic, JXWidget::kVElastic);
	addPathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	removePathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	choosePathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);

	window->SetTitle("Search Paths for Symbol Database");
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(kJFalse);
	SetButtons(okButton, cancelButton);

	itsTable =
		new CBPathTable(dirList, addPathButton, removePathButton,
						choosePathButton, csf,
						scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTable != NULL );
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBEditSearchPathsDialog::ReadPrefs
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
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBEditSearchPathsDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);
}
