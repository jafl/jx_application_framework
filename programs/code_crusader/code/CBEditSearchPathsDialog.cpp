/******************************************************************************
 CBEditSearchPathsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

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
	JXDialogDirector(supervisor, true),
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

	auto* window = jnew JXWindow(this, 370,370, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,100, 240,220);
	assert( scrollbarSet != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,340, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 230,340, 70,20);
	assert( okButton != nullptr );

	auto* instrText =
		jnew JXStaticText(JGetString("instrText::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 330,70);
	assert( instrText != nullptr );

	auto* addPathButton =
		jnew JXTextButton(JGetString("addPathButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,110, 70,20);
	assert( addPathButton != nullptr );
	addPathButton->SetShortcuts(JGetString("addPathButton::CBEditSearchPathsDialog::shortcuts::JXLayout"));

	auto* removePathButton =
		jnew JXTextButton(JGetString("removePathButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,140, 70,20);
	assert( removePathButton != nullptr );
	removePathButton->SetShortcuts(JGetString("removePathButton::CBEditSearchPathsDialog::shortcuts::JXLayout"));

	auto* choosePathButton =
		jnew JXTextButton(JGetString("choosePathButton::CBEditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,190, 70,20);
	assert( choosePathButton != nullptr );
	choosePathButton->SetShortcuts(JGetString("choosePathButton::CBEditSearchPathsDialog::shortcuts::JXLayout"));

// end JXLayout

	instrText->GetText()->SetText(JGetString("Instructions::CBEditSearchPathsDialog"));
	window->AdjustSize(0, instrText->GetBoundsHeight() - instrText->GetFrameHeight());
	instrText->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);
	scrollbarSet->SetSizing(JXWidget::kHElastic, JXWidget::kVElastic);
	addPathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	removePathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	choosePathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);

	window->SetTitle(JGetString("WindowTitle::CBEditSearchPathsDialog"));
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(false);
	SetButtons(okButton, cancelButton);

	itsTable =
		jnew CBPathTable(dirList, addPathButton, removePathButton,
						choosePathButton, csf,
						scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTable != nullptr );
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBEditSearchPathsDialog::ReadPrefs
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
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBEditSearchPathsDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);
}
