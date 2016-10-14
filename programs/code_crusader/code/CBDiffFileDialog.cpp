/******************************************************************************
 CBDiffFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBDiffFileDialog.h"
#include "CBDiffDocument.h"
#include "CBDiffStyleMenu.h"
#include "CBSVNFileInput.h"
#include "CBCommandManager.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include "cbHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXTextMenu.h>
#include <JXFileInput.h>
#include <JXStaticText.h>
#include <JXTabGroup.h>
#include <JXDocumentMenu.h>
#include <JXHelpManager.h>
#include <JXColormap.h>
#include <JXChooseSaveFile.h>
#include <JProcess.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jWebUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <strstream>
#include <jAssert.h>

static const JCharacter* kCVSRev1MenuStr =
	"  Current  %r"
	"| Previous %r"
	"| Tag      %r"
	"| Date     %r";

static const JCharacter* kCVSRev2MenuStr =
	"  Edited  %r"
	"| Current %r"
	"| Tag     %r"
	"| Date    %r";

static const JCharacter* kSVNRev1MenuStr =
	"    Current      %r"
	"  | Previous     %r"
	"  | Revision     %r"
	"  | Date (Y-M-D) %r"
	"%l| Trunk        %r"
	"  | Branch       %r"
	"  | Tag          %r";

static const JCharacter* kSVNRev2MenuStr =
	"    Edited       %r"
	"  | Current      %r"
	"  | Revision     %r"
	"  | Date (Y-M-D) %r"
	"%l| Trunk        %r"
	"  | Branch       %r"
	"  | Tag          %r";

static const JCharacter* kGitRev1MenuStr =
	"    Current  %r"
	"  | Previous %r"
	"  | Commit   %r"
	"  | Date     %r"
	"  | Branch   %r"
	"  | Common ancestor of %r";

static const JCharacter* kGitRev2MenuStr =
	"    Edited  %r"
	"  | Current %r"
	"  | Commit  %r"
	"  | Date    %r"
	"  | Branch  %r";

enum
{
	kCurrentRevCmd = 1,
	kPreviousRevCmd,
	kRevisionNumberCmd,
	kRevisionDateCmd,

	// SVN

	kTrunkCmd,
	kBranchCmd,
	kTagCmd,

	// git

	kGitBranchCmd    = kTrunkCmd,
	kGit1AncestorCmd = kBranchCmd
};

inline JBoolean
cbIsFixedRevCmd
	(
	const JIndex cmd
	)
{
	return JI2B(cmd == kCurrentRevCmd ||
				cmd == kPreviousRevCmd);
}

inline JBoolean
cbIsSVNFixedRevCmd
	(
	const JIndex cmd
	)
{
	return JI2B(cmd == kCurrentRevCmd  ||
				cmd == kPreviousRevCmd ||
				cmd == kTrunkCmd);
}

// setup information

const JFileVersion kCurrentSetupVersion = 2;

	// version 2 stores itsIgnoreSpaceChangeCB, itsIgnoreBlankLinesCB
	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDiffFileDialog::CBDiffFileDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBDiffFileDialogID)
{
	itsTabIndex   = kPlainDiffTabIndex;
	itsCVSRev1Cmd = kCurrentRevCmd;
	itsCVSRev2Cmd = kCurrentRevCmd;
	itsSVNRev1Cmd = kCurrentRevCmd;
	itsSVNRev2Cmd = kCurrentRevCmd;
	itsGitRev1Cmd = kCurrentRevCmd;
	itsGitRev2Cmd = kCurrentRevCmd;

	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDiffFileDialog::~CBDiffFileDialog()
{
	// prefs written by CBDeleteGlobals()
}

/******************************************************************************
 Set file

 ******************************************************************************/

void
CBDiffFileDialog::SetFile1
	(
	const JCharacter* fullName
	)
{
	itsTabGroup->ShowTab(kPlainDiffTabIndex);
	itsPlainFile1Input->SetText(fullName);
	Activate();
	itsPlainFile1Input->Focus();
}

void
CBDiffFileDialog::SetFile2
	(
	const JCharacter* fullName
	)
{
	itsTabGroup->ShowTab(kPlainDiffTabIndex);
	itsPlainFile2Input->SetText(fullName);
	Activate();
	itsPlainFile2Input->Focus();
}

void
CBDiffFileDialog::SetCVSFile
	(
	const JCharacter* fullName
	)
{
	itsTabGroup->ShowTab(kCVSDiffTabIndex);
	itsCVSFileInput->SetText(fullName);
	Activate();
	itsCVSFileInput->Focus();
}

void
CBDiffFileDialog::SetSVNFile
	(
	const JCharacter* fullName
	)
{
	itsTabGroup->ShowTab(kSVNDiffTabIndex);
	itsSVNFileInput->SetText(fullName);
	Activate();
	itsSVNFileInput->Focus();
}

void
CBDiffFileDialog::SetGitFile
	(
	const JCharacter* fullName
	)
{
	itsTabGroup->ShowTab(kGitDiffTabIndex);
	itsGitFileInput->SetText(fullName);
	Activate();
	itsGitFileInput->Focus();
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBDiffFileDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 550,210, "");
	assert( window != NULL );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,175, 60,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::CBDiffFileDialog::shortcuts::JXLayout"));

	itsViewButton =
		jnew JXTextButton(JGetString("itsViewButton::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 460,175, 60,20);
	assert( itsViewButton != NULL );
	itsViewButton->SetShortcuts(JGetString("itsViewButton::CBDiffFileDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,175, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBDiffFileDialog::shortcuts::JXLayout"));

	itsCommonStyleMenu =
		jnew CBDiffStyleMenu("Shared text style", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 140,30);
	assert( itsCommonStyleMenu != NULL );

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,175, 90,20);
	assert( itsStayOpenCB != NULL );

	itsIgnoreSpaceChangeCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreSpaceChangeCB::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 270,20);
	assert( itsIgnoreSpaceChangeCB != NULL );

	itsIgnoreBlankLinesCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreBlankLinesCB::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,140, 230,20);
	assert( itsIgnoreBlankLinesCB != NULL );

	itsTabGroup =
		jnew JXTabGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 530,120);
	assert( itsTabGroup != NULL );

// end JXLayout

	JXWidgetSet* plainCard = itsTabGroup->AppendTab("Normal");
	JXWidgetSet* cvsCard   = itsTabGroup->AppendTab("  CVS  ");
	JXWidgetSet* svnCard   = itsTabGroup->AppendTab("Subversion");
	JXWidgetSet* gitCard   = itsTabGroup->AppendTab("    Git    ");

// begin plainLayout

	const JRect plainLayout_Frame    = plainCard->GetFrame();
	const JRect plainLayout_Aperture = plainCard->GetAperture();
	plainCard->AdjustSize(530 - plainLayout_Aperture.width(), 70 - plainLayout_Aperture.height());

	JXStaticText* obj1_plainLayout =
		jnew JXStaticText(JGetString("obj1_plainLayout::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 20,20);
	assert( obj1_plainLayout != NULL );
	obj1_plainLayout->SetToLabel();

	itsPlainOnly1StyleMenu =
		jnew CBDiffStyleMenu("Style", plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,10, 70,20);
	assert( itsPlainOnly1StyleMenu != NULL );

	itsPlainOnly2StyleMenu =
		jnew CBDiffStyleMenu("Style", plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,40, 70,20);
	assert( itsPlainOnly2StyleMenu != NULL );

	itsPlainFile1Input =
		jnew JXFileInput(plainCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 30,10, 320,20);
	assert( itsPlainFile1Input != NULL );

	itsPlainChoose1Button =
		jnew JXTextButton(JGetString("itsPlainChoose1Button::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,10, 60,20);
	assert( itsPlainChoose1Button != NULL );

	JXStaticText* obj2_plainLayout =
		jnew JXStaticText(JGetString("obj2_plainLayout::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 20,20);
	assert( obj2_plainLayout != NULL );
	obj2_plainLayout->SetToLabel();

	itsPlainFile2Input =
		jnew JXFileInput(plainCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 30,40, 320,20);
	assert( itsPlainFile2Input != NULL );

	itsPlainChoose2Button =
		jnew JXTextButton(JGetString("itsPlainChoose2Button::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,40, 60,20);
	assert( itsPlainChoose2Button != NULL );

	plainCard->SetSize(plainLayout_Frame.width(), plainLayout_Frame.height());

// end plainLayout

// begin cvsLayout

	const JRect cvsLayout_Frame    = cvsCard->GetFrame();
	const JRect cvsLayout_Aperture = cvsCard->GetAperture();
	cvsCard->AdjustSize(510 - cvsLayout_Aperture.width(), 90 - cvsLayout_Aperture.height());

	itsCVSFileInput =
		jnew JXFileInput(cvsCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	assert( itsCVSFileInput != NULL );

	itsCVSOnly1StyleMenu =
		jnew CBDiffStyleMenu("Style", cvsCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);
	assert( itsCVSOnly1StyleMenu != NULL );

	itsCVSOnly2StyleMenu =
		jnew CBDiffStyleMenu("Style", cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);
	assert( itsCVSOnly2StyleMenu != NULL );

	itsCVSChooseButton =
		jnew JXTextButton(JGetString("itsCVSChooseButton::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);
	assert( itsCVSChooseButton != NULL );

	itsCVSRev1Input =
		jnew JXInputField(cvsCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);
	assert( itsCVSRev1Input != NULL );

	itsCVSRev1Menu =
		jnew JXTextMenu(JGetString("itsCVSRev1Menu::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);
	assert( itsCVSRev1Menu != NULL );

	itsCVSRev2Input =
		jnew JXInputField(cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);
	assert( itsCVSRev2Input != NULL );

	itsCVSRev2Menu =
		jnew JXTextMenu(JGetString("itsCVSRev2Menu::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);
	assert( itsCVSRev2Menu != NULL );

	itsCVSSummaryCB =
		jnew JXTextCheckbox(JGetString("itsCVSSummaryCB::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);
	assert( itsCVSSummaryCB != NULL );

	cvsCard->SetSize(cvsLayout_Frame.width(), cvsLayout_Frame.height());

// end cvsLayout

// begin svnLayout

	const JRect svnLayout_Frame    = svnCard->GetFrame();
	const JRect svnLayout_Aperture = svnCard->GetAperture();
	svnCard->AdjustSize(510 - svnLayout_Aperture.width(), 90 - svnLayout_Aperture.height());

	itsSVNFileInput =
		jnew CBSVNFileInput(svnCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	assert( itsSVNFileInput != NULL );

	itsSVNOnly1StyleMenu =
		jnew CBDiffStyleMenu("Style", svnCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);
	assert( itsSVNOnly1StyleMenu != NULL );

	itsSVNOnly2StyleMenu =
		jnew CBDiffStyleMenu("Style", svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);
	assert( itsSVNOnly2StyleMenu != NULL );

	itsSVNChooseButton =
		jnew JXTextButton(JGetString("itsSVNChooseButton::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);
	assert( itsSVNChooseButton != NULL );

	itsSVNRev1Input =
		jnew JXInputField(svnCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);
	assert( itsSVNRev1Input != NULL );

	itsSVNRev1Menu =
		jnew JXTextMenu(JGetString("itsSVNRev1Menu::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);
	assert( itsSVNRev1Menu != NULL );

	itsSVNRev2Input =
		jnew JXInputField(svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);
	assert( itsSVNRev2Input != NULL );

	itsSVNRev2Menu =
		jnew JXTextMenu(JGetString("itsSVNRev2Menu::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);
	assert( itsSVNRev2Menu != NULL );

	itsSVNSummaryCB =
		jnew JXTextCheckbox(JGetString("itsSVNSummaryCB::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);
	assert( itsSVNSummaryCB != NULL );

	svnCard->SetSize(svnLayout_Frame.width(), svnLayout_Frame.height());

// end svnLayout

// begin gitLayout

	const JRect gitLayout_Frame    = gitCard->GetFrame();
	const JRect gitLayout_Aperture = gitCard->GetAperture();
	gitCard->AdjustSize(510 - gitLayout_Aperture.width(), 90 - gitLayout_Aperture.height());

	itsGitFileInput =
		jnew JXFileInput(gitCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	assert( itsGitFileInput != NULL );

	itsGitOnly1StyleMenu =
		jnew CBDiffStyleMenu("Style", gitCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);
	assert( itsGitOnly1StyleMenu != NULL );

	itsGitOnly2StyleMenu =
		jnew CBDiffStyleMenu("Style", gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);
	assert( itsGitOnly2StyleMenu != NULL );

	itsGitChooseButton =
		jnew JXTextButton(JGetString("itsGitChooseButton::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);
	assert( itsGitChooseButton != NULL );

	itsGitRev1Input =
		jnew JXInputField(gitCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);
	assert( itsGitRev1Input != NULL );

	itsGitRev1Menu =
		jnew JXTextMenu(JGetString("itsGitRev1Menu::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);
	assert( itsGitRev1Menu != NULL );

	itsGitRev2Input =
		jnew JXInputField(gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);
	assert( itsGitRev2Input != NULL );

	itsGitRev2Menu =
		jnew JXTextMenu(JGetString("itsGitRev2Menu::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);
	assert( itsGitRev2Menu != NULL );

	itsGitSummaryCB =
		jnew JXTextCheckbox(JGetString("itsGitSummaryCB::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);
	assert( itsGitSummaryCB != NULL );

	gitCard->SetSize(gitLayout_Frame.width(), gitLayout_Frame.height());

// end gitLayout

	window->SetTitle("Compare files");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(kJTrue);

	assert( kTabCount == 4 );
	itsStyleMenu[0][0] = itsPlainOnly1StyleMenu;
	itsStyleMenu[0][1] = itsPlainOnly2StyleMenu;
	itsStyleMenu[1][0] = itsCVSOnly1StyleMenu;
	itsStyleMenu[1][1] = itsCVSOnly2StyleMenu;
	itsStyleMenu[2][0] = itsSVNOnly1StyleMenu;
	itsStyleMenu[2][1] = itsSVNOnly2StyleMenu;
	itsStyleMenu[3][0] = itsGitOnly1StyleMenu;
	itsStyleMenu[3][1] = itsGitOnly2StyleMenu;

	ListenTo(itsTabGroup->GetCardEnclosure());
	ListenTo(itsViewButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsPlainChoose1Button);
	ListenTo(itsPlainChoose2Button);
	ListenTo(itsCVSChooseButton);
	ListenTo(itsSVNChooseButton);
	ListenTo(itsGitChooseButton);

	// plain

	itsPlainFile1Input->ShouldAllowInvalidFile(kJTrue);
	itsPlainFile1Input->ShouldRequireWritable(kJFalse);
	itsPlainFile1Input->ShouldBroadcastAllTextChanged(kJTrue);
	itsPlainFile1Input->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsPlainFile1Input);

	itsPlainFile2Input->ShouldAllowInvalidFile(kJTrue);
	itsPlainFile2Input->ShouldRequireWritable(kJFalse);
	itsPlainFile2Input->ShouldBroadcastAllTextChanged(kJTrue);
	itsPlainFile2Input->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsPlainFile2Input);

	JXColormap* cmap = GetColormap();
	itsPlainOnly1StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJTrue, cmap->GetRedColor()));
	itsPlainOnly2StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJFalse, cmap->GetBlueColor()));
	itsCommonStyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJFalse, cmap->GetGrayColor(50)));

	// CVS

	itsCVSRev1Menu->SetMenuItems(kCVSRev1MenuStr);
	itsCVSRev1Menu->SetToPopupChoice(kJTrue, itsCVSRev1Cmd);
	itsCVSRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCVSRev1Menu);

	itsCVSRev2Menu->SetMenuItems(kCVSRev2MenuStr);
	itsCVSRev2Menu->SetToPopupChoice(kJTrue, itsCVSRev2Cmd);
	itsCVSRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCVSRev2Menu);

	itsCVSOnly1StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJTrue, cmap->GetRedColor()));
	itsCVSOnly2StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJFalse, cmap->GetBlueColor()));

	itsCVSRev1Input->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsCVSRev1Input);

	itsCVSRev2Input->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsCVSRev2Input);

	itsCVSFileInput->ShouldAllowInvalidFile(kJTrue);
	itsCVSFileInput->ShouldRequireWritable(kJFalse);
	itsCVSFileInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsCVSFileInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCVSFileInput);

	itsCVSChooseButton->SetHint(JGetString("ChoseButtonHint::CBDiffFileDialog"));

	// SVN

	itsSVNRev1Menu->SetMenuItems(kSVNRev1MenuStr);
	itsSVNRev1Menu->SetToPopupChoice(kJTrue, itsSVNRev1Cmd);
	itsSVNRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSVNRev1Menu);

	itsSVNRev2Menu->SetMenuItems(kSVNRev2MenuStr);
	itsSVNRev2Menu->SetToPopupChoice(kJTrue, itsSVNRev2Cmd);
	itsSVNRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSVNRev2Menu);

	itsSVNOnly1StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJTrue, cmap->GetRedColor()));
	itsSVNOnly2StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJFalse, cmap->GetBlueColor()));

	itsSVNRev1Input->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsSVNRev1Input);

	itsSVNRev2Input->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsSVNRev2Input);

	itsSVNFileInput->ShouldAllowInvalidFile(kJTrue);
	itsSVNFileInput->ShouldRequireWritable(kJFalse);
	itsSVNFileInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsSVNFileInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsSVNFileInput);

	itsSVNChooseButton->SetHint(JGetString("ChoseButtonHint::CBDiffFileDialog"));

	// git

	itsGitRev1Menu->SetMenuItems(kGitRev1MenuStr);
	itsGitRev1Menu->SetToPopupChoice(kJTrue, itsGitRev1Cmd);
	itsGitRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsGitRev1Menu);

	itsGitRev2Menu->SetMenuItems(kGitRev2MenuStr);
	itsGitRev2Menu->SetToPopupChoice(kJTrue, itsGitRev2Cmd);
	itsGitRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsGitRev2Menu);

	itsGitOnly1StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJTrue, cmap->GetRedColor()));
	itsGitOnly2StyleMenu->SetStyle(
		JFontStyle(kJFalse, kJFalse, 0, kJFalse, cmap->GetBlueColor()));

	itsGitRev1Input->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsGitRev1Input);

	itsGitRev2Input->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsGitRev2Input);

	itsGitFileInput->ShouldAllowInvalidFile(kJTrue);
	itsGitFileInput->ShouldRequireWritable(kJFalse);
	itsGitFileInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsGitFileInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsGitFileInput);

	itsGitChooseButton->SetHint(JGetString("ChoseButtonHint::CBDiffFileDialog"));

	// misc

	itsStayOpenCB->SetState(kJTrue);

	UpdateBasePath();
	ListenTo(CBGetDocumentManager());

	// create hidden JXDocument so Meta-# shortcuts work

	JXDocumentMenu* fileListMenu =
		jnew JXDocumentMenu("", window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != NULL );

	HandleGitRevMenu(itsGitRev1Menu, kCurrentRevCmd, &itsGitRev1Cmd, itsGitRev1Input);
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBDiffFileDialog::UpdateDisplay()
{
	JString s;
	if (itsTabIndex == kPlainDiffTabIndex)
		{
		itsViewButton->SetActive(JI2B(
			itsPlainFile1Input->GetFile(&s) && itsPlainFile2Input->GetFile(&s)));
		}
	else if (itsTabIndex == kCVSDiffTabIndex)
		{
		itsViewButton->SetActive(JI2B(
			CheckVCSFileOrPath(itsCVSFileInput, kJFalse, NULL) &&
			(cbIsFixedRevCmd(itsCVSRev1Cmd) || !itsCVSRev1Input->IsEmpty()) &&
			(cbIsFixedRevCmd(itsCVSRev2Cmd) || !itsCVSRev2Input->IsEmpty())));

		itsCVSRev1Input->SetActive(!cbIsFixedRevCmd(itsCVSRev1Cmd));
		itsCVSRev2Menu->SetActive(JI2B(itsCVSRev1Cmd != kCurrentRevCmd));
		itsCVSRev2Input->SetActive(!cbIsFixedRevCmd(itsCVSRev2Cmd));
		}
	else if (itsTabIndex == kSVNDiffTabIndex)
		{
		itsViewButton->SetActive(JI2B(
			CheckSVNFileOrPath(itsSVNFileInput, kJFalse, NULL) &&
			(cbIsSVNFixedRevCmd(itsSVNRev1Cmd) || !itsSVNRev1Input->IsEmpty()) &&
			(cbIsSVNFixedRevCmd(itsSVNRev2Cmd) || !itsSVNRev2Input->IsEmpty())));

		itsSVNRev1Input->SetActive(!cbIsSVNFixedRevCmd(itsSVNRev1Cmd));
		itsSVNRev2Menu->SetActive(JI2B(itsSVNRev1Cmd != kCurrentRevCmd));
		itsSVNRev2Input->SetActive(!cbIsSVNFixedRevCmd(itsSVNRev2Cmd));
		}
	else
		{
		assert( itsTabIndex == kGitDiffTabIndex );

		itsViewButton->SetActive(JI2B(
			CheckVCSFileOrPath(itsGitFileInput, kJFalse, NULL) &&
			(cbIsFixedRevCmd(itsGitRev1Cmd) || !itsGitRev1Input->IsEmpty()) &&
			(cbIsFixedRevCmd(itsGitRev2Cmd) || !itsGitRev2Input->IsEmpty())));

		itsGitRev1Input->SetActive(!cbIsFixedRevCmd(itsGitRev1Cmd));
		itsGitRev2Menu->SetActive(JI2B(itsGitRev1Cmd != kCurrentRevCmd));
		itsGitRev2Input->SetActive(!cbIsFixedRevCmd(itsGitRev2Cmd));
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBDiffFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsViewButton && message.Is(JXButton::kPushed))
		{
		if (ViewDiffs() && !itsStayOpenCB->IsChecked())
			{
			Deactivate();
			}
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBDiffHelpName);
		}

	else if (sender == itsPlainChoose1Button && message.Is(JXButton::kPushed))
		{
		ChooseFile(itsPlainFile1Input);
		}
	else if (sender == itsPlainChoose2Button && message.Is(JXButton::kPushed))
		{
		ChooseFile(itsPlainFile2Input);
		}

	else if ((sender == itsPlainFile1Input ||
			  sender == itsPlainFile2Input ||
			  sender == itsCVSFileInput    ||
			  sender == itsCVSRev1Input    ||
			  sender == itsCVSRev2Input    ||
			  sender == itsSVNFileInput    ||
			  sender == itsSVNRev1Input    ||
			  sender == itsSVNRev2Input    ||
			  sender == itsGitFileInput    ||
			  sender == itsGitRev1Input    ||
			  sender == itsGitRev2Input) &&
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
		{
		UpdateDisplay();
		}

	else if (sender == itsCVSChooseButton && message.Is(JXButton::kPushed))
		{
		if ((GetDisplay()->GetLatestKeyModifiers()).meta())
			{
			ChoosePath(itsCVSFileInput);
			}
		else
			{
			ChooseFile(itsCVSFileInput);
			}
		}

	else if (sender == itsSVNChooseButton && message.Is(JXButton::kPushed))
		{
		if ((GetDisplay()->GetLatestKeyModifiers()).meta())
			{
			ChoosePath(itsSVNFileInput);
			}
		else
			{
			ChooseFile(itsSVNFileInput);
			}
		}

	else if (sender == itsGitChooseButton && message.Is(JXButton::kPushed))
		{
		if ((GetDisplay()->GetLatestKeyModifiers()).meta())
			{
			ChoosePath(itsGitFileInput);
			}
		else
			{
			ChooseFile(itsGitFileInput);
			}
		}

	else if (sender == itsCVSRev1Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsCVSRev1Menu, itsCVSRev1Cmd);
		}
	else if (sender == itsCVSRev1Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleCVSRevMenu(itsCVSRev1Menu, selection->GetIndex(), &itsCVSRev1Cmd, itsCVSRev1Input);
		}

	else if (sender == itsCVSRev2Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsCVSRev2Menu, itsCVSRev2Cmd);
		}
	else if (sender == itsCVSRev2Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleCVSRevMenu(itsCVSRev2Menu, selection->GetIndex(), &itsCVSRev2Cmd, itsCVSRev2Input);
		}

	else if (sender == itsSVNRev1Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsSVNRev1Menu, itsSVNRev1Cmd);
		}
	else if (sender == itsSVNRev1Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleSVNRevMenu(itsSVNRev1Menu, selection->GetIndex(), &itsSVNRev1Cmd, itsSVNRev1Input);
		}

	else if (sender == itsSVNRev2Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsSVNRev2Menu, itsSVNRev2Cmd);
		}
	else if (sender == itsSVNRev2Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleSVNRevMenu(itsSVNRev2Menu, selection->GetIndex(), &itsSVNRev2Cmd, itsSVNRev2Input);
		}

	else if (sender == itsGitRev1Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsGitRev1Menu, itsGitRev1Cmd);
		}
	else if (sender == itsGitRev1Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleGitRevMenu(itsGitRev1Menu, selection->GetIndex(), &itsGitRev1Cmd, itsGitRev1Input);
		}

	else if (sender == itsGitRev2Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsGitRev2Menu, itsGitRev2Cmd);
		}
	else if (sender == itsGitRev2Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleGitRevMenu(itsGitRev2Menu, selection->GetIndex(), &itsGitRev2Cmd, itsGitRev2Input);
		}

	else if (sender == itsTabGroup->GetCardEnclosure() &&
			 message.Is(JXCardFile::kCardIndexChanged))
		{
		JIndex index;
		if (itsTabGroup->GetCurrentTabIndex(&index) && index != itsTabIndex)
			{
			for (JIndex i=1; i<=kTabCount; i++)
				{
				if (i != itsTabIndex)
					{
					itsStyleMenu[i-1][0]->SetStyle(
						itsStyleMenu[itsTabIndex-1][0]->GetStyle());
					itsStyleMenu[i-1][1]->SetStyle(
						itsStyleMenu[itsTabIndex-1][1]->GetStyle());
					}
				}
			itsTabIndex = index;
			}
		UpdateDisplay();
		}

	else if (sender == CBGetDocumentManager() &&
			 message.Is(CBDocumentManager::kProjectDocumentActivated))
		{
		UpdateBasePath();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ViewDiffs (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::ViewDiffs()
{
	JIndex tabIndex;
	const JBoolean ok = itsTabGroup->GetCurrentTabIndex(&tabIndex);
	assert( ok );

	JString fullName;

	if (tabIndex == kPlainDiffTabIndex &&
		CheckFile(itsPlainFile1Input) &&
		CheckFile(itsPlainFile2Input))
		{
		itsPlainFile1Input->DeactivateCurrentUndo();		// undo from latest button press
		itsPlainFile2Input->DeactivateCurrentUndo();

		JString file1, file2;
		JBoolean ok = itsPlainFile1Input->GetFile(&file1);
		assert( ok );
		ok = itsPlainFile2Input->GetFile(&file2);
		assert( ok );

		ViewDiffs(file1, file2);
		return kJTrue;
		}
	else if (tabIndex == kCVSDiffTabIndex &&
			 CheckVCSFileOrPath(itsCVSFileInput, kJTrue, &fullName))
		{
		itsCVSFileInput->DeactivateCurrentUndo();			// undo from latest button press
		itsCVSRev1Input->DeactivateCurrentUndo();
		itsCVSRev2Input->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
			{
			DiffDirectory(fullName, "cvs -f diff", itsCVSSummaryCB, " --brief");
			}
		else
			{
			JString getCmd, diffCmd, name1, name2;
			if (BuildCVSDiffCmd(fullName, itsCVSRev1Cmd, itsCVSRev1Input->GetText(),
								itsCVSRev2Cmd, itsCVSRev2Input->GetText(),
								&getCmd, &diffCmd, &name1, &name2, kJFalse))
				{
				const JError err =
					CBDiffDocument::CreateCVS(fullName, getCmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsCVSOnly1StyleMenu->GetStyle(),
											  name2, itsCVSOnly2StyleMenu->GetStyle(),
											  kJFalse);
				err.ReportIfError();
				}
			}
		return kJTrue;
		}
	else if (tabIndex == kSVNDiffTabIndex &&
			 CheckSVNFileOrPath(itsSVNFileInput, kJTrue, &fullName))
		{
		itsSVNFileInput->DeactivateCurrentUndo();			// undo from latest button press
		itsSVNRev1Input->DeactivateCurrentUndo();
		itsSVNRev2Input->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
			{
			const JString s = JCombinePathAndName(fullName, ".") + ".";
			JString getCmd, diffCmd, name1, name2;
			if (BuildSVNDiffCmd(s,
								itsSVNRev1Cmd, itsSVNRev1Input->GetText(),
								itsSVNRev2Cmd, itsSVNRev2Input->GetText(),
								&getCmd, &diffCmd, &name1, &name2, kJFalse, kJTrue))
				{
				DiffDirectory(fullName, diffCmd, itsSVNSummaryCB, " --diff-cmd diff -x --brief");
				}
			}
		else
			{
			JString getCmd, diffCmd, name1, name2;
			if (BuildSVNDiffCmd(fullName,
								itsSVNRev1Cmd, itsSVNRev1Input->GetText(),
								itsSVNRev2Cmd, itsSVNRev2Input->GetText(),
								&getCmd, &diffCmd, &name1, &name2, kJFalse))
				{
				const JError err =
					CBDiffDocument::CreateSVN(fullName, getCmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsSVNOnly1StyleMenu->GetStyle(),
											  name2, itsSVNOnly2StyleMenu->GetStyle(),
											  kJFalse);
				err.ReportIfError();
				}
			}
		return kJTrue;
		}
	else if (tabIndex == kGitDiffTabIndex &&
			 CheckVCSFileOrPath(itsGitFileInput, kJTrue, &fullName))
		{
		itsGitFileInput->DeactivateCurrentUndo();			// undo from latest button press
		itsGitRev1Input->DeactivateCurrentUndo();
		itsGitRev2Input->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
			{
			JString diffCmd;
			if (BuildGitDiffDirectoryCmd(fullName,
										 itsGitRev1Cmd, itsGitRev1Input->GetText(),
										 itsGitRev2Cmd, itsGitRev2Input->GetText(),
										 &diffCmd))
				{
				if (itsGitSummaryCB->IsChecked())
					{
					diffCmd += " --summary --stat";
					}
				diffCmd += " .";	// force selected directory
				DiffDirectory(fullName, diffCmd, itsGitSummaryCB, "");
				}
			}
		else
			{
			JString get1Cmd, get2Cmd, diffCmd, name1, name2;
			if (BuildGitDiffCmd(fullName,
								itsGitRev1Cmd, itsGitRev1Input->GetText(),
								itsGitRev2Cmd, itsGitRev2Input->GetText(),
								&get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, kJFalse))
				{
				const JError err =
					CBDiffDocument::CreateGit(fullName, get1Cmd, get2Cmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsGitOnly1StyleMenu->GetStyle(),
											  name2, itsGitOnly2StyleMenu->GetStyle(),
											  kJFalse);
				err.ReportIfError();
				}
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 DiffDirectory (private)

 ******************************************************************************/

void
CBDiffFileDialog::DiffDirectory
	(
	const JCharacter*	fullName,
	const JCharacter*	diffCmd,
	JXCheckbox*			summaryCB,
	const JCharacter*	summaryArgs
	)
{
	const JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
	const JArray<JIndex> lineIndexList;

	JString* path = jnew JString(fullName);
	assert( path != NULL );

	JString* cmd = jnew JString(diffCmd);
	assert( cmd != NULL );

	if (summaryCB->IsChecked())
		{
		*cmd += summaryArgs;
		}

	JString* ss = jnew JString;
	assert( ss != NULL );

	JString* mt = jnew JString;
	assert( mt != NULL );

	JString* ms = jnew JString;
	assert( ms != NULL );

	JString* mi = jnew JString;
	assert( mi != NULL );

	CBCommandManager::CmdInfo info(path, cmd, ss, kJFalse, kJTrue, kJTrue, kJTrue,
								   kJTrue, kJTrue, kJFalse, mt, ms, mi, kJFalse);

	CBCommandManager::Exec(info, NULL, fullNameList, lineIndexList);

	info.Free();
}

/******************************************************************************
 ViewDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewDiffs
	(
	const JCharacter*	fullName1,
	const JCharacter*	fullName2,
	const JBoolean		silent
	)
{
	if (CBDocumentManager::WarnFileSize(fullName1) &&
		CBDocumentManager::WarnFileSize(fullName2))
		{
		itsTabGroup->ShowTab(kPlainDiffTabIndex);		// update styles

		JString cmd = BuildDiffCmd();
		cmd += JPrepArgForExec(fullName1);
		cmd += " ";
		cmd += JPrepArgForExec(fullName2);

		JString path, name1, name2;
		JSplitPathAndName(fullName1, &path, &name1);
		JSplitPathAndName(fullName2, &path, &name2);

		const JError err =
			CBDiffDocument::CreatePlain(fullName1, cmd, itsCommonStyleMenu->GetStyle(),
										name1, itsPlainOnly1StyleMenu->GetStyle(),
										name2, itsPlainOnly2StyleMenu->GetStyle(),
										silent);
		if (!silent)
			{
			err.ReportIfError();
			}
		}
}

/******************************************************************************
 BuildDiffCmd (private)

	Returns a command to which two file names can be appended.

 ******************************************************************************/

JString
CBDiffFileDialog::BuildDiffCmd()
{
	JString cmd = "diff ";
	if (itsIgnoreSpaceChangeCB->IsChecked())
		{
		cmd += "-b ";		// --ignore-space-change is GNU specific
		}
	if (itsIgnoreBlankLinesCB->IsChecked())
		{
		cmd += "--ignore-blank-lines ";
		}

	return cmd;
}

/******************************************************************************
 ViewCVSDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewCVSDiffs
	(
	const JCharacter*	fullName,
	const JBoolean		silent
	)
{
	if (CBDocumentManager::WarnFileSize(fullName))
		{
		itsTabGroup->ShowTab(kCVSDiffTabIndex);		// update styles

		JString getCmd, diffCmd, name1, name2;
		if (BuildCVSDiffCmd(fullName, itsCVSRev1Cmd, NULL, itsCVSRev2Cmd, NULL,
							&getCmd, &diffCmd, &name1, &name2, silent))
			{
			const JError err =
				CBDiffDocument::CreateCVS(fullName, getCmd, diffCmd,
										  itsCommonStyleMenu->GetStyle(),
										  name1, itsCVSOnly1StyleMenu->GetStyle(),
										  name2, itsCVSOnly2StyleMenu->GetStyle(),
										  silent);
			if (!silent)
				{
				err.ReportIfError();
				}
			}
		}
}

/******************************************************************************
 ViewCVSDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewCVSDiffs
	(
	const JCharacter*	fullName,
	const JCharacter*	rev1,
	const JCharacter*	rev2,
	const JBoolean		silent
	)
{
	if (CBDocumentManager::WarnFileSize(fullName))
		{
		JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
		if (JStringEmpty(rev1))
			{
			rev1Cmd = rev2Cmd = kCurrentRevCmd;
			}
		else if (JStringEmpty(rev2))
			{
			rev2Cmd = kCurrentRevCmd;
			}

		JString getCmd, diffCmd, name1, name2;
		if (BuildCVSDiffCmd(fullName, rev1Cmd, rev1, rev2Cmd, rev2,
							&getCmd, &diffCmd, &name1, &name2, silent))
			{
			const JError err =
				CBDiffDocument::CreateCVS(fullName, getCmd, diffCmd,
										  itsCommonStyleMenu->GetStyle(),
										  name1, itsCVSOnly1StyleMenu->GetStyle(),
										  name2, itsCVSOnly2StyleMenu->GetStyle(),
										  silent);
			err.ReportIfError();
			}
		}
}

/******************************************************************************
 BuildCVSDiffCmd (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::BuildCVSDiffCmd
	(
	const JCharacter*	fullName,
	const JIndex		rev1Cmd,
	const JCharacter*	origRev1,
	const JIndex		rev2Cmd,
	const JCharacter*	origRev2,
	JString*			getCmd,
	JString*			diffCmd,
	JString*			name1,
	JString*			name2,
	const JBoolean		silent
	)
{
	JString path, name, cvsRoot;
	JSplitPathAndName(fullName, &path, &name);

	cvsRoot = JCombinePathAndName(path, "CVS");
	cvsRoot = JCombinePathAndName(cvsRoot, "Root");
	JReadFile(cvsRoot, &cvsRoot);
	cvsRoot.TrimWhitespace();

	*name1  = name;
	*name1 += ": ";

	*getCmd  = "cvs -d " + JPrepArgForExec(cvsRoot) + " get ";
	*diffCmd = "cvs -f diff ";
	if ((rev1Cmd == kPreviousRevCmd && origRev1 != NULL) ||
		(!cbIsFixedRevCmd(rev1Cmd) && !JStringEmpty(origRev1)))
		{
		JIndex cmd1  = rev1Cmd;
		JString rev1 = origRev1;
		if (cmd1 == kPreviousRevCmd && GetPreviousCVSRevision(fullName, &rev1))
			{
			cmd1 = kRevisionNumberCmd;
			}
		else if (cmd1 == kPreviousRevCmd)
			{
			if (!silent)
				{
				(JGetUserNotification())->ReportError(JGetString("VCSNoPreviousRevision::CBDiffFileDialog"));
				}
			return kJFalse;
			}

		JIndex cmd2  = rev2Cmd;
		JString rev2 = origRev2;
		if (cmd2 == kPreviousRevCmd && GetCurrentCVSRevision(fullName, &rev2))
			{
			cmd2 = kRevisionNumberCmd;
			}
		else if (cmd2 == kPreviousRevCmd)
			{
			cmd2 = kCurrentRevCmd;
			}

		*getCmd += (cmd1 == kRevisionNumberCmd ? "-r " : "-D ");
		*getCmd += JPrepArgForExec(rev1);
		*getCmd += " ";

		*diffCmd += (cmd1 == kRevisionNumberCmd ? "-r " : "-D ");
		*diffCmd += JPrepArgForExec(rev1);
		*diffCmd += " ";

		*name1 += rev1;

		if (cmd2 != kCurrentRevCmd && !JStringEmpty(rev2))
			{
			*diffCmd += (cmd2 == kRevisionNumberCmd ? "-r " : "-D ");
			*diffCmd += JPrepArgForExec(rev2);
			*diffCmd += " ";

			*name2 = rev2;
			}
		else
			{
			*name2 = "edited";
			}
		}
	else
		{
		*name1 += "current";
		*name2  = "edited";

		JString rev;
		if (GetCurrentCVSRevision(fullName, &rev))
			{
			*getCmd += "-r ";
			*getCmd += JPrepArgForExec(rev);
			*getCmd += " ";
			}
		}

	if (itsIgnoreSpaceChangeCB->IsChecked())
		{
		*diffCmd += "-b ";		// --ignore-space-change is GNU specific
		}
	if (itsIgnoreBlankLinesCB->IsChecked())
		{
		*diffCmd += "--ignore-blank-lines ";
		}
	*diffCmd += JPrepArgForExec(name);

	return kJTrue;
}

/******************************************************************************
 GetCurrentCVSRevision (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::GetCurrentCVSRevision
	(
	const JCharacter*	fullName,
	JString*			rev
	)
{
	JString path, name, data, pattern;
	JSplitPathAndName(fullName, &path, &name);
	pattern = "^[^/]*/" + JRegex::BackslashForLiteral(name) + "/([0-9.]+)/";
	name    = JCombinePathAndName(path, "CVS");
	name    = JCombinePathAndName(name, "Entries");
	JReadFile(name, &data);

	JRegex r(pattern);
	JArray<JIndexRange> matchList;
	if (r.Match(data, &matchList))
		{
		*rev = data.GetSubstring(matchList.GetElement(2));
		return kJTrue;
		}
	else
		{
		rev->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 GetPreviousCVSRevision (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::GetPreviousCVSRevision
	(
	const JCharacter*	fullName,
	JString*			rev
	)
{
	if (!GetCurrentCVSRevision(fullName, rev))
		{
		return kJFalse;
		}

	JIndex i;
	if (!rev->LocateLastSubstring(".", &i) || i == rev->GetLength())
		{
		return kJFalse;
		}

	JIndex j = atoi(((const JCharacter*) *rev) + i);
	if (j > 1)
		{
		rev->RemoveSubstring(i+1, rev->GetLength());
		*rev += JString(j-1, JString::kBase10);
		return kJTrue;
		}

	i--;
	if (!rev->LocatePrevSubstring(".", &i))
		{
		return kJFalse;
		}

	rev->RemoveSubstring(i+1, rev->GetLength());
	return kJTrue;
}

/******************************************************************************
 ViewSVNDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewSVNDiffs
	(
	const JCharacter*	fullName,
	const JBoolean		silent
	)
{
	if (CBDocumentManager::WarnFileSize(fullName))
		{
		itsTabGroup->ShowTab(kSVNDiffTabIndex);		// update styles

		JString getCmd, diffCmd, name1, name2;
		if (BuildSVNDiffCmd(fullName, itsSVNRev1Cmd, NULL, itsSVNRev2Cmd, NULL,
							&getCmd, &diffCmd, &name1, &name2, silent))
			{
			const JError err =
				CBDiffDocument::CreateSVN(fullName, getCmd, diffCmd,
										  itsCommonStyleMenu->GetStyle(),
										  name1, itsSVNOnly1StyleMenu->GetStyle(),
										  name2, itsSVNOnly2StyleMenu->GetStyle(),
										  silent);
			if (!silent)
				{
				err.ReportIfError();
				}
			}
		}
}

/******************************************************************************
 ViewSVNDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewSVNDiffs
	(
	const JCharacter*	fullName,
	const JCharacter*	rev1,
	const JCharacter*	rev2,
	const JBoolean		silent
	)
{
	if (CBDocumentManager::WarnFileSize(fullName))
		{
		JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
		if (JStringEmpty(rev1))
			{
			rev1Cmd = rev2Cmd = kCurrentRevCmd;
			}
		else if (JStringEmpty(rev2))
			{
			rev2Cmd = kCurrentRevCmd;
			}

		JString getCmd, diffCmd, name1, name2;
		if (BuildSVNDiffCmd(fullName, rev1Cmd, rev1, rev2Cmd, rev2,
							&getCmd, &diffCmd, &name1, &name2, silent))
			{
			const JError err =
				CBDiffDocument::CreateSVN(fullName, getCmd, diffCmd,
										  itsCommonStyleMenu->GetStyle(),
										  name1, itsSVNOnly1StyleMenu->GetStyle(),
										  name2, itsSVNOnly2StyleMenu->GetStyle(),
										  silent);
			err.ReportIfError();
			}
		}
}

/******************************************************************************
 BuildSVNDiffCmd (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::BuildSVNDiffCmd
	(
	const JCharacter*	fullName,
	const JIndex		rev1Cmd,
	const JCharacter*	origRev1,
	const JIndex		rev2Cmd,
	const JCharacter*	origRev2,
	JString*			getCmd,
	JString*			diffCmd,
	JString*			name1,
	JString*			name2,
	const JBoolean		silent,
	const JBoolean		forDirectory
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	*name1  = name;
	*name1 += ": ";

	JString file1 = fullName;
	JString file2 = fullName;

	*getCmd  = "svn cat ";
	*diffCmd = (forDirectory ? "svn diff " : "svn diff --diff-cmd diff -x --normal ");
	if ((rev1Cmd == kPreviousRevCmd && origRev1 != NULL) ||		// PREV from dialog
		(rev1Cmd == kTrunkCmd && origRev1 != NULL) ||			// TRUNK from dialog
		(!cbIsSVNFixedRevCmd(rev1Cmd) && !JStringEmpty(origRev1)))
		{
		JIndex cmd1    = rev1Cmd;
		JString rev1   = origRev1;
		JString getRev = origRev1;
		JString revName;
		if (cmd1 == kPreviousRevCmd)
			{
			cmd1 = kRevisionNumberCmd;
			rev1 = "PREV";

			if (forDirectory)
				{
				getRev = "PREV";
				}
			else if (!GetPreviousSVNRevision(fullName, &getRev))
				{
				if (!silent)
					{
					(JGetUserNotification())->ReportError(JGetString("VCSNoPreviousRevision::CBDiffFileDialog"));
					}
				return kJFalse;
				}

			*name1 += rev1;
			}
		else if (cmd1 == kRevisionDateCmd)
			{
			getRev = rev1 = "{" + rev1 + "}";
			*name1 += rev1;
			}
		else if (cmd1 == kTrunkCmd  ||
				 cmd1 == kBranchCmd ||
				 cmd1 == kTagCmd)
			{
			if (!BuildSVNRepositoryPath(&file1, cmd1, rev1, &revName, silent))
				{
				return kJFalse;
				}
			getRev = rev1 = "HEAD";
			*name1 += revName;
			}
		else
			{
			*name1 += rev1;
			}

		JIndex cmd2  = rev2Cmd;
		JString rev2 = origRev2;
		if (cmd2 == kPreviousRevCmd)
			{
			cmd2   = kRevisionNumberCmd;
			rev2   = "COMMITTED";
			*name2 = rev2;
			}
		else if (cmd2 == kRevisionNumberCmd)
			{
			*name2 = rev2;
			}
		else if (cmd2 == kRevisionDateCmd)
			{
			rev2   = "{" + rev2 + "}";
			*name2 = rev2;
			}
		else if (cmd2 == kTrunkCmd  ||
				 cmd2 == kBranchCmd ||
				 cmd2 == kTagCmd)
			{
			if (!BuildSVNRepositoryPath(&file2, cmd2, rev2, &revName, silent))
				{
				return kJFalse;
				}
			rev2   = "HEAD";
			*name2 = revName;
			}
		else if (JIsURL(fullName))
			{
			*name2 = "current";
			}
		else
			{
			*name2 = "edited";
			}

		*getCmd += "-r ";
		*getCmd += JPrepArgForExec(getRev);
		*getCmd += " ";
		*getCmd += JPrepArgForExec(file1);

		*diffCmd += "-r ";
		*diffCmd += JPrepArgForExec(rev1);

		if (cmd2 != kCurrentRevCmd && !JStringEmpty(rev2))
			{
			*diffCmd += ":";
			*diffCmd += JPrepArgForExec(rev2);
			}
		}
	else
		{
		*name1 += "current";
		*name2  = "edited";
		*getCmd = CBDiffDocument::theLocalSVNGetMarker;
		}

	if (itsIgnoreSpaceChangeCB->IsChecked())
		{
		*diffCmd += " -x -b ";		// --ignore-space-change is GNU specific
		}
	if (itsIgnoreBlankLinesCB->IsChecked())
		{
		*diffCmd += " -x --ignore-blank-lines ";
		}

	if (!forDirectory && file1 == file2)
		{
		*diffCmd += " ";
		*diffCmd += JPrepArgForExec(file1);
		}
	else
		{
		*diffCmd += " --old=";
		*diffCmd += JPrepArgForExec(file1);
		*diffCmd += " --new=";
		*diffCmd += JPrepArgForExec(file2);
		}

	return kJTrue;
}

/******************************************************************************
 GetPreviousSVNRevision (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::GetPreviousSVNRevision
	(
	const JCharacter*	fullName,
	JString*			rev
	)
{
	if (!JGetCurrentSVNRevision(fullName, rev))
		{
		return kJFalse;
		}

	JIndex r = atoi(*rev);
	if (r == 1)
		{
		rev->Clear();
		return kJFalse;
		}
	else
		{
		*rev = JString(r-1, JString::kBase10);
		return kJTrue;
		}
}

/******************************************************************************
 BuildSVNRepositoryPath (private)

 ******************************************************************************/

static const JRegex theBranchPattern = "(?<=/)(trunk($|(?=/))|(branches|tags)/[^/]+(?=/))";

JBoolean
CBDiffFileDialog::BuildSVNRepositoryPath
	(
	JString*			fullName,
	const JIndex		cmd,
	const JCharacter*	rev,
	JString*			name,
	const JBoolean		silent
	)
{
	JString repoPath;
	if (!JGetVCSRepositoryPath(*fullName, &repoPath))
		{
		if (!silent)
			{
			(JGetUserNotification())->ReportError(JGetString("SVNNoRepository::CBDiffFileDialog"));
			}
		return kJFalse;
		}

	JIndexRange r;
	if (!theBranchPattern.Match(repoPath, &r))
		{
		if (!silent)
			{
			(JGetUserNotification())->ReportError(JGetString("SVNNonstandardRepository::CBDiffFileDialog"));
			}
		return kJFalse;
		}

	name->Clear();
	if (cmd == kTrunkCmd)
		{
		*name = "trunk";
		}
	else if (cmd == kBranchCmd)
		{
		*name  = "branches/";
		*name += rev;
		}
	else if (cmd == kTagCmd)
		{
		*name  = "tags/";
		*name += rev;
		}
	assert( !name->IsEmpty() );

	repoPath.ReplaceSubstring(r, *name, NULL);
	*fullName = repoPath;
	return kJTrue;
}

/******************************************************************************
 ViewGitDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewGitDiffs
	(
	const JCharacter*	fullName,
	const JBoolean		silent
	)
{
	if (CBDocumentManager::WarnFileSize(fullName))
		{
		itsTabGroup->ShowTab(kGitDiffTabIndex);		// update styles

		JString get1Cmd, get2Cmd, diffCmd, name1, name2;
		if (BuildGitDiffCmd(fullName, itsGitRev1Cmd, NULL, itsGitRev2Cmd, NULL,
							&get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, silent))
			{
			const JError err =
				CBDiffDocument::CreateGit(fullName, get1Cmd, get2Cmd, diffCmd,
										  itsCommonStyleMenu->GetStyle(),
										  name1, itsGitOnly1StyleMenu->GetStyle(),
										  name2, itsGitOnly2StyleMenu->GetStyle(),
										  silent);
			if (!silent)
				{
				err.ReportIfError();
				}
			}
		}
}

/******************************************************************************
 ViewGitDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewGitDiffs
	(
	const JCharacter*	fullName,
	const JCharacter*	rev1,
	const JCharacter*	rev2,
	const JBoolean		silent
	)
{
	if (CBDocumentManager::WarnFileSize(fullName))
		{
		JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
		if (JStringEmpty(rev1))
			{
			rev1Cmd = kCurrentRevCmd;
			rev2Cmd = kCurrentRevCmd;
			}
		else if (JStringEmpty(rev2))
			{
			rev2Cmd = kCurrentRevCmd;
			}

		JString get1Cmd, get2Cmd, diffCmd, name1, name2;
		if (BuildGitDiffCmd(fullName, rev1Cmd, rev1, rev2Cmd, rev2,
							&get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, silent))
			{
			const JError err =
				CBDiffDocument::CreateGit(fullName, get1Cmd, get2Cmd, diffCmd,
										  itsCommonStyleMenu->GetStyle(),
										  name1, itsGitOnly1StyleMenu->GetStyle(),
										  name2, itsGitOnly2StyleMenu->GetStyle(),
										  silent);
			err.ReportIfError();
			}
		}
}

/******************************************************************************
 BuildGitDiffCmd (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::BuildGitDiffCmd
	(
	const JCharacter*	fullName,
	const JIndex		rev1Cmd,
	const JCharacter*	rev1,
	const JIndex		rev2Cmd,
	const JCharacter*	rev2,
	JString*			get1Cmd,
	JString*			get2Cmd,
	JString*			diffCmd,
	JString*			name1,
	JString*			name2,
	const JBoolean		silent
	)
{
	JString trueName;
	if (!JGetTrueName(fullName, &trueName))
		{
		return kJFalse;
		}

	JString path, name;
	JSplitPathAndName(trueName, &path, &name);

	// git show must be run with path relative to repo root

	JString gitRoot;
	if (!JSearchGitRoot(path, &gitRoot))
		{
		return kJFalse;
		}

	JString gitFile = JConvertToRelativePath(trueName, gitRoot);
	if (gitFile.BeginsWith("." ACE_DIRECTORY_SEPARATOR_STR))
		{
		gitFile.RemoveSubstring(1, 2);	// ACE_DIRECTORY_SEPARATOR_CHAR guarantees length 1
		}

	*name1  = name;
	*name1 += ": ";

	get1Cmd->Clear();
	get2Cmd->Clear();
	if ((rev1Cmd == kPreviousRevCmd && rev1 != NULL) ||		// PREV from dialog
		(!cbIsFixedRevCmd(rev1Cmd) && !JStringEmpty(rev1)))
		{
		JString get1Rev;
		if (rev1Cmd == kPreviousRevCmd)
			{
			if (!GetPreviousGitRevision(trueName, &get1Rev))
				{
				if (!silent)
					{
					(JGetUserNotification())->ReportError(JGetString("VCSNoPreviousRevision::CBDiffFileDialog"));
					}
				return kJFalse;
				}

			*name1 += "previous";
			}
		else if (rev1Cmd == kRevisionDateCmd)
			{
			get1Rev  = "@{";
			get1Rev += rev1;
			get1Rev += "}";

			*name1 += get1Rev;
			}
		else if (rev1Cmd != kGit1AncestorCmd)
			{
			get1Rev = rev1;
			*name1 += rev1;
			}

		JString get2Rev;
		if (rev2Cmd == kCurrentRevCmd)
			{
			*get2Cmd  = "file:";
			*get2Cmd += trueName;

			*name2 = "edited";
			}
		else if (rev2Cmd == kPreviousRevCmd)
			{
			*name2 = "current";
			}
		else if (rev2Cmd == kRevisionDateCmd)
			{
			get2Rev  = "@{";
			get2Rev += rev2;
			get2Rev += "}";

			*name2 = get2Rev;
			}
		else
			{
			get2Rev = rev2;
			*name2  = rev2;
			}

		if (rev1Cmd == kGit1AncestorCmd)
			{
			get1Rev = rev1;
			if (!GetBestCommonGitAncestor(path, &get1Rev, get2Rev))
				{
				if (!silent)
					{
					(JGetUserNotification())->ReportError(JGetString("GitNoCommonAncestor::CBDiffFileDialog"));
					}
				return kJFalse;
				}

			*name1 += get1Rev;
			}

		const JString n = JPrepArgForExec(gitFile);

		*get1Cmd  = "git show ";
		*get1Cmd += JPrepArgForExec(get1Rev);
		*get1Cmd += ":";
		*get1Cmd += n;

		if (get2Cmd->IsEmpty())
			{
			*get2Cmd  = "git show ";
			*get2Cmd += JPrepArgForExec(get2Rev);
			*get2Cmd += ":";
			*get2Cmd += n;
			}
		}
	else
		{
		*name1 += "current";
		*name2  = "edited";

		*get1Cmd  = "git show :";
		*get1Cmd += JPrepArgForExec(gitFile);

		*get2Cmd  = "file:";
		*get2Cmd += trueName;
		}

	*diffCmd = BuildDiffCmd();
	return kJTrue;
}

/******************************************************************************
 BuildGitDiffDirectoryCmd (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::BuildGitDiffDirectoryCmd
	(
	const JCharacter*	path,
	const JIndex		rev1Cmd,
	const JCharacter*	rev1,
	const JIndex		rev2Cmd,
	const JCharacter*	rev2,
	JString*			diffCmd
	)
{
	JString trueName;
	if (!JGetTrueName(path, &trueName))
		{
		return kJFalse;
		}

	// git show must be run with path relative to repo root

	JString gitRoot;
	if (!JSearchGitRoot(path, &gitRoot))
		{
		return kJFalse;
		}

	*diffCmd = "git diff ";
	if (itsIgnoreSpaceChangeCB->IsChecked())
		{
		*diffCmd += "--ignore-space-change ";
		}

	if ((rev1Cmd == kPreviousRevCmd && rev1 != NULL) ||		// PREV from dialog
		(!cbIsFixedRevCmd(rev1Cmd) && !JStringEmpty(rev1)))
		{
		JString get1Rev;
		if (rev1Cmd == kPreviousRevCmd)
			{
			const JString s = JCombinePathAndName(trueName, ".") + ".";
			if (!GetPreviousGitRevision(s, &get1Rev))
				{
				(JGetUserNotification())->ReportError(JGetString("VCSNoPreviousRevision::CBDiffFileDialog"));
				return kJFalse;
				}
			}
		else if (rev1Cmd == kRevisionDateCmd)
			{
			get1Rev  = "@{";
			get1Rev += rev1;
			get1Rev += "}";
			}
		else
			{
			get1Rev = rev1;
			}

		*diffCmd += JPrepArgForExec(get1Rev);
		*diffCmd += (rev1Cmd == kGit1AncestorCmd ? "..." : " ");

		JString get2Rev;
		if (rev2Cmd == kCurrentRevCmd)
			{
			// leave blank
			}
		else if (rev2Cmd == kPreviousRevCmd)
			{
			const JString s = JCombinePathAndName(trueName, ".") + ".";
			if (!GetCurrentGitRevision(s, &get1Rev))
				{
				(JGetUserNotification())->ReportError(JGetString("VCSNoCurrentRevision::CBDiffFileDialog"));
				return kJFalse;
				}
			}
		else if (rev2Cmd == kRevisionDateCmd)
			{
			get2Rev  = "@{";
			get2Rev += rev2;
			get2Rev += "}";
			}
		else
			{
			get2Rev = rev2;
			}

		*diffCmd += JPrepArgForExec(get2Rev);
		}

	return kJTrue;
}

/******************************************************************************
 GetCurrentGitRevision (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::GetCurrentGitRevision
	(
	const JCharacter*	fullName,
	JString*			rev
	)
{
	JString s;
	return GetLatestGitRevisions(fullName, rev, &s);
}

/******************************************************************************
 GetPreviousGitRevision (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::GetPreviousGitRevision
	(
	const JCharacter*	fullName,
	JString*			rev
	)
{
	JString s;
	return GetLatestGitRevisions(fullName, &s, rev);
}

/******************************************************************************
 GetLatestGitRevisions (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::GetLatestGitRevisions
	(
	const JCharacter*	fullName,
	JString*			rev1,
	JString*			rev2
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JCharacter* args[] =
		{ "git", "log", "-2", "--format=oneline", name, NULL };

	JProcess* p;
	int fromFD;
	const JError err = JProcess::Create(&p, path, args, sizeof(args),
										kJIgnoreConnection, NULL,
										kJCreatePipe, &fromFD);
	if (err.OK())
		{
		*rev1 = JReadUntil(fromFD, ' ');	// current commit
		JIgnoreUntil(fromFD, '\n');			// ignore comment
		*rev2 = JReadUntil(fromFD, ' ');	// previous commit
		close(fromFD);
		return kJTrue;
		}
	else
		{
		err.ReportIfError();
		rev1->Clear();
		rev2->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 GetBestCommonGitAncestor (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::GetBestCommonGitAncestor
	(
	const JCharacter*	path,
	JString*			rev1,
	const JCharacter*	rev2
	)
{
	if (JStringEmpty(rev2))
		{
		rev2 = "HEAD";
		}

	const JCharacter* args[] =
		{ "git", "merge-base", rev1->GetCString(), rev2, NULL };

	JProcess* p;
	int fromFD;
	const JError err = JProcess::Create(&p, path, args, sizeof(args),
										kJIgnoreConnection, NULL,
										kJCreatePipe, &fromFD);
	if (err.OK())
		{
		if (!JReadAll(fromFD, rev1))
			{
			return kJFalse;
			}
		close(fromFD);
		rev1->TrimWhitespace();
		return kJTrue;
		}
	else
		{
		err.ReportIfError();
		rev1->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 GetSmartDiffItemText

 ******************************************************************************/

const JCharacter*
CBDiffFileDialog::GetSmartDiffItemText
	(
	const JBoolean		onDisk,
	const JCharacter*	fullName,
	JBoolean*			enable
	)
	const
{
	if (!onDisk)
		{
		*enable = kJFalse;
		return JGetString("DiffBackupItemText::CBDiffFileDialog");
		}

	JString path, origFileName;
	JSplitPathAndName(fullName, &path, &origFileName);

	JBoolean isSafetySave, isBackup;
	JString fileName = GetSmartDiffInfo(origFileName, &isSafetySave, &isBackup);

	const JCharacter* id = NULL;
	if (isSafetySave)
		{
		const JString savedName = JCombinePathAndName(path, fileName);
		*enable = JFileReadable(savedName);
		id      = "DiffSavedItemText::CBDiffFileDialog";
		}
	else if (isBackup)
		{
		const JString newName = JCombinePathAndName(path, fileName);
		*enable = JFileReadable(newName);
		id      = "DiffNewItemText::CBDiffFileDialog";
		}
	else
		{
		JString backupName = fullName;
		backupName.AppendCharacter('~');
		*enable = JFileReadable(backupName);
		id      = "DiffBackupItemText::CBDiffFileDialog";
		}
	assert( id != NULL );
	return JGetString(id);
}

/******************************************************************************
 ViewDiffs

	We require onDisk to avoid being passed an invalid fullName.

 ******************************************************************************/

void
CBDiffFileDialog::ViewDiffs
	(
	const JBoolean		onDisk,
	const JCharacter*	fullName,
	const JBoolean		silent
	)
{
	if (!onDisk)
		{
		return;
		}

	JString path, origFileName;
	JSplitPathAndName(fullName, &path, &origFileName);

	JBoolean isSafetySave, isBackup;
	JString fileName = GetSmartDiffInfo(origFileName, &isSafetySave, &isBackup);

	if (isSafetySave)
		{
		const JString savedName = JCombinePathAndName(path, fileName);
		ViewDiffs(savedName, fullName, silent);
		}
	else if (isBackup)
		{
		const JString newName = JCombinePathAndName(path, fileName);
		ViewDiffs(fullName, newName, silent);
		}
	else
		{
		JString backupName = fullName;
		backupName.AppendCharacter('~');
		if (JFileExists(backupName))
			{
			ViewDiffs(backupName, fullName, silent);
			}
		}
}

/******************************************************************************
 GetSmartDiffInfo (private)

	Returns the name of the file without # or ~

 ******************************************************************************/

JString
CBDiffFileDialog::GetSmartDiffInfo
	(
	const JCharacter*	origFileName,
	JBoolean*			isSafetySave,
	JBoolean*			isBackup
	)
	const
{
	JString fileName = origFileName;

	*isSafetySave = kJFalse;
	while (fileName.BeginsWith("#") && fileName.EndsWith("#"))
		{
		fileName.RemoveSubstring(1,1);
		fileName.RemoveSubstring(fileName.GetLength(), fileName.GetLength());
		*isSafetySave = kJTrue;
		}

	*isBackup = kJFalse;
	while (!(*isSafetySave) && fileName.EndsWith("~"))		// don't strip if safety save
		{
		fileName.RemoveSubstring(fileName.GetLength(), fileName.GetLength());
		*isBackup = kJTrue;
		}

	return fileName;
}

/******************************************************************************
 CheckFile (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::CheckFile
	(
	JXFileInput* widget
	)
	const
{
	widget->ShouldAllowInvalidFile(kJFalse);
	const JBoolean valid = widget->InputValid();
	widget->ShouldAllowInvalidFile(kJTrue);
	if (!valid)
		{
		widget->Focus();
		}
	return valid;
}

/******************************************************************************
 ChooseFile (private)

 ******************************************************************************/

void
CBDiffFileDialog::ChooseFile
	(
	JXFileInput* widget
	)
{
	const JString origName = widget->GetTextForChooseFile();
	JString newName;
	if ((JGetChooseSaveFile())->ChooseFile("", NULL, origName, &newName))
		{
		widget->Focus();
		widget->SetText(newName);
		}
}

/******************************************************************************
 ChoosePath (private)

 ******************************************************************************/

void
CBDiffFileDialog::ChoosePath
	(
	JXFileInput* widget
	)
{
	const JString origPath = widget->GetTextForChooseFile();
	JString newPath;
	if ((JGetChooseSaveFile())->ChooseRPath("", NULL, origPath, &newPath))
		{
		widget->Focus();
		widget->SetText(newPath);
		}
}

/******************************************************************************
 CheckVCSFileOrPath (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::CheckVCSFileOrPath
	(
	JXFileInput*	widget,
	const JBoolean	reportError,
	JString*		fullName		// can be NULL
	)
	const
{
	const JString& text = widget->GetText();
	if (!text.IsEmpty())
		{
		JString basePath, s;
		widget->GetBasePath(&basePath);
		if (JConvertToAbsolutePath(text, basePath, &s))
			{
			if (fullName != NULL)
				{
				*fullName = s;
				}
			return kJTrue;
			}
		}

	if (reportError)
		{
		widget->Focus();
		(JGetUserNotification())->ReportError(JGetString("VCSDiffTargetInvalid::CBDiffFileDialog"));
		}

	if (fullName != NULL)
		{
		fullName->Clear();
		}
	return kJFalse;
}

/******************************************************************************
 UpdateVCSRevMenu (private)

 ******************************************************************************/

void
CBDiffFileDialog::UpdateVCSRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	cmd
	)
{
	menu->CheckItem(cmd);
}

/******************************************************************************
 HandleCVSRevMenu (private)

 ******************************************************************************/

void
CBDiffFileDialog::HandleCVSRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JIndex*			cmd,
	JXInputField*	input
	)
{
	*cmd = index;

	if (menu == itsCVSRev1Menu && index == kCurrentRevCmd)
		{
		itsCVSRev2Cmd = kCurrentRevCmd;
		itsCVSRev2Menu->SetPopupChoice(itsCVSRev2Cmd);
		}

	UpdateDisplay();
	input->Focus();
}

/******************************************************************************
 CheckSVNFileOrPath (private)

 ******************************************************************************/

JBoolean
CBDiffFileDialog::CheckSVNFileOrPath
	(
	JXFileInput*	widget,
	const JBoolean	reportError,
	JString*		fullName		// can be NULL
	)
	const
{
	const JString& text = widget->GetText();
	if (!text.IsEmpty())
		{
		if (JIsURL(text))
			{
			if (fullName != NULL)
				{
				*fullName = text;
				}
			return kJTrue;
			}

		JString basePath, s;
		widget->GetBasePath(&basePath);
		if (JConvertToAbsolutePath(text, basePath, &s))
			{
			if (fullName != NULL)
				{
				*fullName = s;
				}
			return kJTrue;
			}
		}

	if (reportError)
		{
		widget->Focus();
		(JGetUserNotification())->ReportError(JGetString("SVNDiffTargetInvalid::CBDiffFileDialog"));
		}

	if (fullName != NULL)
		{
		fullName->Clear();
		}
	return kJFalse;
}

/******************************************************************************
 HandleSVNRevMenu (private)

 ******************************************************************************/

void
CBDiffFileDialog::HandleSVNRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JIndex*			cmd,
	JXInputField*	input
	)
{
	*cmd = index;

	if (menu == itsSVNRev1Menu && index == kCurrentRevCmd)
		{
		itsSVNRev2Cmd = kCurrentRevCmd;
		itsSVNRev2Menu->SetPopupChoice(itsSVNRev2Cmd);
		}

	UpdateDisplay();
	input->Focus();
}

/******************************************************************************
 HandleGitRevMenu (private)

 ******************************************************************************/

void
CBDiffFileDialog::HandleGitRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JIndex*			cmd,
	JXInputField*	input
	)
{
	*cmd = index;

	if (menu == itsGitRev1Menu && index == kCurrentRevCmd)
		{
		itsGitRev2Cmd = kCurrentRevCmd;
		itsGitRev2Menu->SetPopupChoice(itsGitRev2Cmd);
		}

	UpdateDisplay();
	input->Focus();
}

/******************************************************************************
 UpdateBasePath (private)

 ******************************************************************************/

void
CBDiffFileDialog::UpdateBasePath()
{
	CBProjectDocument* doc;
	if ((CBGetDocumentManager())->GetActiveProjectDocument(&doc))
		{
		const JString& path = doc->GetFilePath();
		itsPlainFile1Input->SetBasePath(path);
		itsPlainFile2Input->SetBasePath(path);
		itsCVSFileInput->SetBasePath(path);
		itsSVNFileInput->SetBasePath(path);
		}
	else
		{
		itsPlainFile1Input->ClearBasePath();
		itsPlainFile2Input->ClearBasePath();
		itsCVSFileInput->ClearBasePath();
		itsSVNFileInput->ClearBasePath();
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBDiffFileDialog::ReadPrefs
	(
	istream& input
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

	itsCommonStyleMenu->ReadStyle(input);

	itsPlainOnly1StyleMenu->ReadStyle(input);
	itsPlainOnly2StyleMenu->ReadStyle(input);

	itsCVSOnly1StyleMenu->SetStyle(itsPlainOnly1StyleMenu->GetStyle());
	itsCVSOnly2StyleMenu->SetStyle(itsPlainOnly2StyleMenu->GetStyle());

	itsSVNOnly1StyleMenu->SetStyle(itsPlainOnly1StyleMenu->GetStyle());
	itsSVNOnly2StyleMenu->SetStyle(itsPlainOnly2StyleMenu->GetStyle());

	itsGitOnly1StyleMenu->SetStyle(itsPlainOnly1StyleMenu->GetStyle());
	itsGitOnly2StyleMenu->SetStyle(itsPlainOnly2StyleMenu->GetStyle());

	if (vers >= 2)
		{
		JBoolean ignoreSpaceChange, ignoreBlankLines;
		input >> ignoreSpaceChange >> ignoreBlankLines;
		itsIgnoreSpaceChangeCB->SetState(ignoreSpaceChange);
		itsIgnoreBlankLinesCB->SetState(ignoreBlankLines);
		}

	if (vers >= 1)
		{
		JBoolean stayOpen;
		input >> stayOpen;
		itsStayOpenCB->SetState(stayOpen);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBDiffFileDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsCommonStyleMenu->WriteStyle(output);

	if (itsTabIndex == kPlainDiffTabIndex)
		{
		output << ' ';
		itsPlainOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsPlainOnly2StyleMenu->WriteStyle(output);
		}
	else if (itsTabIndex == kCVSDiffTabIndex)
		{
		output << ' ';
		itsCVSOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsCVSOnly2StyleMenu->WriteStyle(output);
		}
	else if (itsTabIndex == kSVNDiffTabIndex)
		{
		output << ' ';
		itsSVNOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsSVNOnly2StyleMenu->WriteStyle(output);
		}
	else
		{
		assert( itsTabIndex == kGitDiffTabIndex );

		output << ' ';
		itsGitOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsGitOnly2StyleMenu->WriteStyle(output);
		}

	output << ' ' << itsIgnoreSpaceChangeCB->IsChecked();
	output << ' ' << itsIgnoreBlankLinesCB->IsChecked();

	output << ' ' << itsStayOpenCB->IsChecked();
	output << ' ';
}
