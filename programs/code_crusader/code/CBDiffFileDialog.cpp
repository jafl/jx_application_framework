/******************************************************************************
 CBDiffFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBDiffFileDialog.h"
#include "CBDiffDocument.h"
#include "CBDiffStyleMenu.h"
#include "CBSVNFileInput.h"
#include "CBCommandManager.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXTextMenu.h>
#include <JXFileInput.h>
#include <JXStaticText.h>
#include <JXTabGroup.h>
#include <JXDocumentMenu.h>
#include <JXHelpManager.h>
#include <JXColorManager.h>
#include <JXChooseSaveFile.h>
#include <JProcess.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jWebUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

static JString kDot(".", JString::kNoCopy);

static const JUtf8Byte* kCVSRev1MenuStr =
	"  Current  %r"
	"| Previous %r"
	"| Tag      %r"
	"| Date     %r";

static const JUtf8Byte* kCVSRev2MenuStr =
	"  Edited  %r"
	"| Current %r"
	"| Tag     %r"
	"| Date    %r";

static const JUtf8Byte* kSVNRev1MenuStr =
	"    Current      %r"
	"  | Previous     %r"
	"  | Revision     %r"
	"  | Date (Y-M-D) %r"
	"%l| Trunk        %r"
	"  | Branch       %r"
	"  | Tag          %r";

static const JUtf8Byte* kSVNRev2MenuStr =
	"    Edited       %r"
	"  | Current      %r"
	"  | Revision     %r"
	"  | Date (Y-M-D) %r"
	"%l| Trunk        %r"
	"  | Branch       %r"
	"  | Tag          %r";

static const JUtf8Byte* kGitRev1MenuStr =
	"    Current  %r"
	"  | Previous %r"
	"  | Commit   %r"
	"  | Date     %r"
	"  | Branch   %r"
	"  | Common ancestor of %r";

static const JUtf8Byte* kGitRev2MenuStr =
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

inline bool
cbIsFixedRevCmd
	(
	const JIndex cmd
	)
{
	return cmd == kCurrentRevCmd ||
				cmd == kPreviousRevCmd;
}

inline bool
cbIsSVNFixedRevCmd
	(
	const JIndex cmd
	)
{
	return cmd == kCurrentRevCmd  ||
				cmd == kPreviousRevCmd ||
				cmd == kTrunkCmd;
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
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kPlainDiffTabIndex);
	itsPlainFile1Input->GetText()->SetText(fullName);
	Activate();
	itsPlainFile1Input->Focus();
}

void
CBDiffFileDialog::SetFile2
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kPlainDiffTabIndex);
	itsPlainFile2Input->GetText()->SetText(fullName);
	Activate();
	itsPlainFile2Input->Focus();
}

void
CBDiffFileDialog::SetCVSFile
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kCVSDiffTabIndex);
	itsCVSFileInput->GetText()->SetText(fullName);
	Activate();
	itsCVSFileInput->Focus();
}

void
CBDiffFileDialog::SetSVNFile
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kSVNDiffTabIndex);
	itsSVNFileInput->GetText()->SetText(fullName);
	Activate();
	itsSVNFileInput->Focus();
}

void
CBDiffFileDialog::SetGitFile
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kGitDiffTabIndex);
	itsGitFileInput->GetText()->SetText(fullName);
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

	auto* window = jnew JXWindow(this, 550,210, JString::empty);
	assert( window != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,175, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::CBDiffFileDialog::shortcuts::JXLayout"));

	itsViewButton =
		jnew JXTextButton(JGetString("itsViewButton::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 460,175, 60,20);
	assert( itsViewButton != nullptr );
	itsViewButton->SetShortcuts(JGetString("itsViewButton::CBDiffFileDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,175, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBDiffFileDialog::shortcuts::JXLayout"));

	itsCommonStyleMenu =
		jnew CBDiffStyleMenu(JGetString("SharedStyleMenuTitle::CBDiffFileDialog"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 140,30);
	assert( itsCommonStyleMenu != nullptr );

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,175, 90,20);
	assert( itsStayOpenCB != nullptr );

	itsIgnoreSpaceChangeCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreSpaceChangeCB::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 270,20);
	assert( itsIgnoreSpaceChangeCB != nullptr );

	itsIgnoreBlankLinesCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreBlankLinesCB::CBDiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,140, 230,20);
	assert( itsIgnoreBlankLinesCB != nullptr );

	itsTabGroup =
		jnew JXTabGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 530,120);
	assert( itsTabGroup != nullptr );

// end JXLayout

	JXContainer* plainCard = itsTabGroup->AppendTab(JGetString("PlainTabLabel::CBDiffFileDialog"));
	JXContainer* cvsCard   = itsTabGroup->AppendTab(JGetString("CVSTabLabel::CBDiffFileDialog"));
	JXContainer* svnCard   = itsTabGroup->AppendTab(JGetString("SVNTabLabel::CBDiffFileDialog"));
	JXContainer* gitCard   = itsTabGroup->AppendTab(JGetString("GitTabLabel::CBDiffFileDialog"));

// begin plainLayout

	const JRect plainLayout_Frame    = plainCard->GetFrame();
	const JRect plainLayout_Aperture = plainCard->GetAperture();
	plainCard->AdjustSize(530 - plainLayout_Aperture.width(), 70 - plainLayout_Aperture.height());

	auto* file1Label =
		jnew JXStaticText(JGetString("file1Label::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 20,20);
	assert( file1Label != nullptr );
	file1Label->SetToLabel();

	itsPlainOnly1StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,10, 70,20);
	assert( itsPlainOnly1StyleMenu != nullptr );

	itsPlainOnly2StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,40, 70,20);
	assert( itsPlainOnly2StyleMenu != nullptr );

	itsPlainFile1Input =
		jnew JXFileInput(plainCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 30,10, 320,20);
	assert( itsPlainFile1Input != nullptr );

	itsPlainChoose1Button =
		jnew JXTextButton(JGetString("itsPlainChoose1Button::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,10, 60,20);
	assert( itsPlainChoose1Button != nullptr );

	auto* file2Label =
		jnew JXStaticText(JGetString("file2Label::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 20,20);
	assert( file2Label != nullptr );
	file2Label->SetToLabel();

	itsPlainFile2Input =
		jnew JXFileInput(plainCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 30,40, 320,20);
	assert( itsPlainFile2Input != nullptr );

	itsPlainChoose2Button =
		jnew JXTextButton(JGetString("itsPlainChoose2Button::CBDiffFileDialog::plainLayout"), plainCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,40, 60,20);
	assert( itsPlainChoose2Button != nullptr );

	plainCard->SetSize(plainLayout_Frame.width(), plainLayout_Frame.height());

// end plainLayout

// begin cvsLayout

	const JRect cvsLayout_Frame    = cvsCard->GetFrame();
	const JRect cvsLayout_Aperture = cvsCard->GetAperture();
	cvsCard->AdjustSize(510 - cvsLayout_Aperture.width(), 90 - cvsLayout_Aperture.height());

	itsCVSFileInput =
		jnew JXFileInput(cvsCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	assert( itsCVSFileInput != nullptr );

	itsCVSOnly1StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), cvsCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);
	assert( itsCVSOnly1StyleMenu != nullptr );

	itsCVSOnly2StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);
	assert( itsCVSOnly2StyleMenu != nullptr );

	itsCVSChooseButton =
		jnew JXTextButton(JGetString("itsCVSChooseButton::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);
	assert( itsCVSChooseButton != nullptr );

	itsCVSRev1Input =
		jnew JXInputField(cvsCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);
	assert( itsCVSRev1Input != nullptr );

	itsCVSRev1Menu =
		jnew JXTextMenu(JGetString("itsCVSRev1Menu::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);
	assert( itsCVSRev1Menu != nullptr );

	itsCVSRev2Input =
		jnew JXInputField(cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);
	assert( itsCVSRev2Input != nullptr );

	itsCVSRev2Menu =
		jnew JXTextMenu(JGetString("itsCVSRev2Menu::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);
	assert( itsCVSRev2Menu != nullptr );

	itsCVSSummaryCB =
		jnew JXTextCheckbox(JGetString("itsCVSSummaryCB::CBDiffFileDialog::cvsLayout"), cvsCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);
	assert( itsCVSSummaryCB != nullptr );

	cvsCard->SetSize(cvsLayout_Frame.width(), cvsLayout_Frame.height());

// end cvsLayout

// begin svnLayout

	const JRect svnLayout_Frame    = svnCard->GetFrame();
	const JRect svnLayout_Aperture = svnCard->GetAperture();
	svnCard->AdjustSize(510 - svnLayout_Aperture.width(), 90 - svnLayout_Aperture.height());

	itsSVNFileInput =
		jnew CBSVNFileInput(svnCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	assert( itsSVNFileInput != nullptr );

	itsSVNOnly1StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), svnCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);
	assert( itsSVNOnly1StyleMenu != nullptr );

	itsSVNOnly2StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);
	assert( itsSVNOnly2StyleMenu != nullptr );

	itsSVNChooseButton =
		jnew JXTextButton(JGetString("itsSVNChooseButton::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);
	assert( itsSVNChooseButton != nullptr );

	itsSVNRev1Input =
		jnew JXInputField(svnCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);
	assert( itsSVNRev1Input != nullptr );

	itsSVNRev1Menu =
		jnew JXTextMenu(JGetString("itsSVNRev1Menu::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);
	assert( itsSVNRev1Menu != nullptr );

	itsSVNRev2Input =
		jnew JXInputField(svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);
	assert( itsSVNRev2Input != nullptr );

	itsSVNRev2Menu =
		jnew JXTextMenu(JGetString("itsSVNRev2Menu::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);
	assert( itsSVNRev2Menu != nullptr );

	itsSVNSummaryCB =
		jnew JXTextCheckbox(JGetString("itsSVNSummaryCB::CBDiffFileDialog::svnLayout"), svnCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);
	assert( itsSVNSummaryCB != nullptr );

	svnCard->SetSize(svnLayout_Frame.width(), svnLayout_Frame.height());

// end svnLayout

// begin gitLayout

	const JRect gitLayout_Frame    = gitCard->GetFrame();
	const JRect gitLayout_Aperture = gitCard->GetAperture();
	gitCard->AdjustSize(510 - gitLayout_Aperture.width(), 90 - gitLayout_Aperture.height());

	itsGitFileInput =
		jnew JXFileInput(gitCard,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	assert( itsGitFileInput != nullptr );

	itsGitOnly1StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), gitCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);
	assert( itsGitOnly1StyleMenu != nullptr );

	itsGitOnly2StyleMenu =
		jnew CBDiffStyleMenu(JGetString("Title::JXStyleMenu"), gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);
	assert( itsGitOnly2StyleMenu != nullptr );

	itsGitChooseButton =
		jnew JXTextButton(JGetString("itsGitChooseButton::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);
	assert( itsGitChooseButton != nullptr );

	itsGitRev1Input =
		jnew JXInputField(gitCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);
	assert( itsGitRev1Input != nullptr );

	itsGitRev1Menu =
		jnew JXTextMenu(JGetString("itsGitRev1Menu::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);
	assert( itsGitRev1Menu != nullptr );

	itsGitRev2Input =
		jnew JXInputField(gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);
	assert( itsGitRev2Input != nullptr );

	itsGitRev2Menu =
		jnew JXTextMenu(JGetString("itsGitRev2Menu::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);
	assert( itsGitRev2Menu != nullptr );

	itsGitSummaryCB =
		jnew JXTextCheckbox(JGetString("itsGitSummaryCB::CBDiffFileDialog::gitLayout"), gitCard,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);
	assert( itsGitSummaryCB != nullptr );

	gitCard->SetSize(gitLayout_Frame.width(), gitLayout_Frame.height());

// end gitLayout

	window->SetTitle(JGetString("WindowTitle::CBDiffFileDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

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

	itsPlainFile1Input->ShouldAllowInvalidFile(true);
	itsPlainFile1Input->ShouldRequireWritable(false);
	itsPlainFile1Input->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsPlainFile1Input);

	itsPlainFile2Input->ShouldAllowInvalidFile(true);
	itsPlainFile2Input->ShouldRequireWritable(false);
	itsPlainFile2Input->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsPlainFile2Input);

	itsPlainOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsPlainOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));
	itsCommonStyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetGrayColor(50)));

	// CVS

	itsCVSRev1Menu->SetMenuItems(kCVSRev1MenuStr);
	itsCVSRev1Menu->SetToPopupChoice(true, itsCVSRev1Cmd);
	itsCVSRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCVSRev1Menu);

	itsCVSRev2Menu->SetMenuItems(kCVSRev2MenuStr);
	itsCVSRev2Menu->SetToPopupChoice(true, itsCVSRev2Cmd);
	itsCVSRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCVSRev2Menu);

	itsCVSOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsCVSOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));

	ListenTo(itsCVSRev1Input);
	ListenTo(itsCVSRev2Input);

	itsCVSFileInput->ShouldAllowInvalidFile(true);
	itsCVSFileInput->ShouldRequireWritable(false);
	itsCVSFileInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCVSFileInput);

	itsCVSChooseButton->SetHint(JGetString("ChoseButtonHint::CBDiffFileDialog"));

	// SVN

	itsSVNRev1Menu->SetMenuItems(kSVNRev1MenuStr);
	itsSVNRev1Menu->SetToPopupChoice(true, itsSVNRev1Cmd);
	itsSVNRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSVNRev1Menu);

	itsSVNRev2Menu->SetMenuItems(kSVNRev2MenuStr);
	itsSVNRev2Menu->SetToPopupChoice(true, itsSVNRev2Cmd);
	itsSVNRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSVNRev2Menu);

	itsSVNOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsSVNOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));

	ListenTo(itsSVNRev1Input);
	ListenTo(itsSVNRev2Input);

	itsSVNFileInput->ShouldAllowInvalidFile(true);
	itsSVNFileInput->ShouldRequireWritable(false);
	itsSVNFileInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsSVNFileInput);

	itsSVNChooseButton->SetHint(JGetString("ChoseButtonHint::CBDiffFileDialog"));

	// git

	itsGitRev1Menu->SetMenuItems(kGitRev1MenuStr);
	itsGitRev1Menu->SetToPopupChoice(true, itsGitRev1Cmd);
	itsGitRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsGitRev1Menu);

	itsGitRev2Menu->SetMenuItems(kGitRev2MenuStr);
	itsGitRev2Menu->SetToPopupChoice(true, itsGitRev2Cmd);
	itsGitRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsGitRev2Menu);

	itsGitOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsGitOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));

	ListenTo(itsGitRev1Input);
	ListenTo(itsGitRev2Input);

	itsGitFileInput->ShouldAllowInvalidFile(true);
	itsGitFileInput->ShouldRequireWritable(false);
	itsGitFileInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsGitFileInput);

	itsGitChooseButton->SetHint(JGetString("ChoseButtonHint::CBDiffFileDialog"));

	// misc

	itsStayOpenCB->SetState(true);

	UpdateBasePath();
	ListenTo(CBGetDocumentManager());

	// create hidden JXDocument so Meta-# shortcuts work

	auto* fileListMenu =
		jnew JXDocumentMenu(JString::empty, window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != nullptr );

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
		itsViewButton->SetActive(itsPlainFile1Input->GetFile(&s) && itsPlainFile2Input->GetFile(&s));
		}
	else if (itsTabIndex == kCVSDiffTabIndex)
		{
		itsViewButton->SetActive(CheckVCSFileOrPath(itsCVSFileInput, false, nullptr) &&
			(cbIsFixedRevCmd(itsCVSRev1Cmd) || !itsCVSRev1Input->GetText()->IsEmpty()) &&
			(cbIsFixedRevCmd(itsCVSRev2Cmd) || !itsCVSRev2Input->GetText()->IsEmpty()));

		itsCVSRev1Input->SetActive(!cbIsFixedRevCmd(itsCVSRev1Cmd));
		itsCVSRev2Menu->SetActive(itsCVSRev1Cmd != kCurrentRevCmd);
		itsCVSRev2Input->SetActive(!cbIsFixedRevCmd(itsCVSRev2Cmd));
		}
	else if (itsTabIndex == kSVNDiffTabIndex)
		{
		itsViewButton->SetActive(CheckSVNFileOrPath(itsSVNFileInput, false, nullptr) &&
			(cbIsSVNFixedRevCmd(itsSVNRev1Cmd) || !itsSVNRev1Input->GetText()->IsEmpty()) &&
			(cbIsSVNFixedRevCmd(itsSVNRev2Cmd) || !itsSVNRev2Input->GetText()->IsEmpty()));

		itsSVNRev1Input->SetActive(!cbIsSVNFixedRevCmd(itsSVNRev1Cmd));
		itsSVNRev2Menu->SetActive(itsSVNRev1Cmd != kCurrentRevCmd);
		itsSVNRev2Input->SetActive(!cbIsSVNFixedRevCmd(itsSVNRev2Cmd));
		}
	else
		{
		assert( itsTabIndex == kGitDiffTabIndex );

		itsViewButton->SetActive(CheckVCSFileOrPath(itsGitFileInput, false, nullptr) &&
			(cbIsFixedRevCmd(itsGitRev1Cmd) || !itsGitRev1Input->GetText()->IsEmpty()) &&
			(cbIsFixedRevCmd(itsGitRev2Cmd) || !itsGitRev2Input->GetText()->IsEmpty()));

		itsGitRev1Input->SetActive(!cbIsFixedRevCmd(itsGitRev1Cmd));
		itsGitRev2Menu->SetActive(itsGitRev1Cmd != kCurrentRevCmd);
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
		(JXGetHelpManager())->ShowSection("CBEditorHelp");
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
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleCVSRevMenu(itsCVSRev1Menu, selection->GetIndex(), &itsCVSRev1Cmd, itsCVSRev1Input);
		}

	else if (sender == itsCVSRev2Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsCVSRev2Menu, itsCVSRev2Cmd);
		}
	else if (sender == itsCVSRev2Menu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleCVSRevMenu(itsCVSRev2Menu, selection->GetIndex(), &itsCVSRev2Cmd, itsCVSRev2Input);
		}

	else if (sender == itsSVNRev1Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsSVNRev1Menu, itsSVNRev1Cmd);
		}
	else if (sender == itsSVNRev1Menu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSVNRevMenu(itsSVNRev1Menu, selection->GetIndex(), &itsSVNRev1Cmd, itsSVNRev1Input);
		}

	else if (sender == itsSVNRev2Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsSVNRev2Menu, itsSVNRev2Cmd);
		}
	else if (sender == itsSVNRev2Menu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSVNRevMenu(itsSVNRev2Menu, selection->GetIndex(), &itsSVNRev2Cmd, itsSVNRev2Input);
		}

	else if (sender == itsGitRev1Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsGitRev1Menu, itsGitRev1Cmd);
		}
	else if (sender == itsGitRev1Menu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleGitRevMenu(itsGitRev1Menu, selection->GetIndex(), &itsGitRev1Cmd, itsGitRev1Input);
		}

	else if (sender == itsGitRev2Menu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVCSRevMenu(itsGitRev2Menu, itsGitRev2Cmd);
		}
	else if (sender == itsGitRev2Menu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
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

bool
CBDiffFileDialog::ViewDiffs()
{
	JIndex tabIndex;
	const bool ok = itsTabGroup->GetCurrentTabIndex(&tabIndex);
	assert( ok );

	JString fullName;

	if (tabIndex == kPlainDiffTabIndex &&
		CheckFile(itsPlainFile1Input) &&
		CheckFile(itsPlainFile2Input))
		{
		itsPlainFile1Input->GetText()->DeactivateCurrentUndo();		// undo from latest button press
		itsPlainFile2Input->GetText()->DeactivateCurrentUndo();

		JString file1, file2;
		bool ok = itsPlainFile1Input->GetFile(&file1);
		assert( ok );
		ok = itsPlainFile2Input->GetFile(&file2);
		assert( ok );

		ViewDiffs(file1, file2);
		return true;
		}
	else if (tabIndex == kCVSDiffTabIndex &&
			 CheckVCSFileOrPath(itsCVSFileInput, true, &fullName))
		{
		itsCVSFileInput->GetText()->DeactivateCurrentUndo();			// undo from latest button press
		itsCVSRev1Input->GetText()->DeactivateCurrentUndo();
		itsCVSRev2Input->GetText()->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
			{
			DiffDirectory(fullName, JString("cvs -f diff", JString::kNoCopy),
						  itsCVSSummaryCB, JString(" --brief", JString::kNoCopy));
			}
		else
			{
			JString getCmd, diffCmd, name1, name2;
			if (BuildCVSDiffCmd(fullName, itsCVSRev1Cmd, itsCVSRev1Input->GetText()->GetText(),
								itsCVSRev2Cmd, itsCVSRev2Input->GetText()->GetText(),
								&getCmd, &diffCmd, &name1, &name2, false))
				{
				const JError err =
					CBDiffDocument::CreateCVS(fullName, getCmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsCVSOnly1StyleMenu->GetStyle(),
											  name2, itsCVSOnly2StyleMenu->GetStyle(),
											  false);
				err.ReportIfError();
				}
			}
		return true;
		}
	else if (tabIndex == kSVNDiffTabIndex &&
			 CheckSVNFileOrPath(itsSVNFileInput, true, &fullName))
		{
		itsSVNFileInput->GetText()->DeactivateCurrentUndo();			// undo from latest button press
		itsSVNRev1Input->GetText()->DeactivateCurrentUndo();
		itsSVNRev2Input->GetText()->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
			{
			const JString s = JCombinePathAndName(fullName, kDot) + kDot;
			JString getCmd, diffCmd, name1, name2;
			if (BuildSVNDiffCmd(s,
								itsSVNRev1Cmd, itsSVNRev1Input->GetText()->GetText(),
								itsSVNRev2Cmd, itsSVNRev2Input->GetText()->GetText(),
								&getCmd, &diffCmd, &name1, &name2, false, true))
				{
				DiffDirectory(fullName, diffCmd, itsSVNSummaryCB, JString(" --diff-cmd diff -x --brief", JString::kNoCopy));
				}
			}
		else
			{
			JString getCmd, diffCmd, name1, name2;
			if (BuildSVNDiffCmd(fullName,
								itsSVNRev1Cmd, itsSVNRev1Input->GetText()->GetText(),
								itsSVNRev2Cmd, itsSVNRev2Input->GetText()->GetText(),
								&getCmd, &diffCmd, &name1, &name2, false))
				{
				const JError err =
					CBDiffDocument::CreateSVN(fullName, getCmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsSVNOnly1StyleMenu->GetStyle(),
											  name2, itsSVNOnly2StyleMenu->GetStyle(),
											  false);
				err.ReportIfError();
				}
			}
		return true;
		}
	else if (tabIndex == kGitDiffTabIndex &&
			 CheckVCSFileOrPath(itsGitFileInput, true, &fullName))
		{
		itsGitFileInput->GetText()->DeactivateCurrentUndo();			// undo from latest button press
		itsGitRev1Input->GetText()->DeactivateCurrentUndo();
		itsGitRev2Input->GetText()->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
			{
			JString diffCmd;
			if (BuildGitDiffDirectoryCmd(fullName,
										 itsGitRev1Cmd, itsGitRev1Input->GetText()->GetText(),
										 itsGitRev2Cmd, itsGitRev2Input->GetText()->GetText(),
										 &diffCmd))
				{
				if (itsGitSummaryCB->IsChecked())
					{
					diffCmd += " --summary --stat";
					}
				diffCmd += " .";	// force selected directory
				DiffDirectory(fullName, diffCmd, itsGitSummaryCB, JString::empty);
				}
			}
		else
			{
			JString get1Cmd, get2Cmd, diffCmd, name1, name2;
			if (BuildGitDiffCmd(fullName,
								itsGitRev1Cmd, itsGitRev1Input->GetText()->GetText(),
								itsGitRev2Cmd, itsGitRev2Input->GetText()->GetText(),
								&get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, false))
				{
				const JError err =
					CBDiffDocument::CreateGit(fullName, get1Cmd, get2Cmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsGitOnly1StyleMenu->GetStyle(),
											  name2, itsGitOnly2StyleMenu->GetStyle(),
											  false);
				err.ReportIfError();
				}
			}
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 DiffDirectory (private)

 ******************************************************************************/

void
CBDiffFileDialog::DiffDirectory
	(
	const JString&	fullName,
	const JString&	diffCmd,
	JXCheckbox*		summaryCB,
	const JString&	summaryArgs
	)
{
	const JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
	const JArray<JIndex> lineIndexList;

	auto* path = jnew JString(fullName);
	assert( path != nullptr );

	auto* cmd = jnew JString(diffCmd);
	assert( cmd != nullptr );

	if (summaryCB->IsChecked())
		{
		*cmd += summaryArgs;
		}

	auto* ss = jnew JString;
	assert( ss != nullptr );

	auto* mt = jnew JString;
	assert( mt != nullptr );

	auto* ms = jnew JString;
	assert( ms != nullptr );

	auto* mi = jnew JString;
	assert( mi != nullptr );

	CBCommandManager::CmdInfo info(path, cmd, ss, false, true, true, true,
								   true, true, false, mt, ms, mi, false);

	CBCommandManager::Exec(info, nullptr, fullNameList, lineIndexList);

	info.Free();
}

/******************************************************************************
 ViewDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewDiffs
	(
	const JString&	fullName1,
	const JString&	fullName2,
	const bool	silent
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
	JString cmd("diff ");
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
 ViewVCSDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewVCSDiffs
	(
	const JString&	fullName,
	const bool	silent
	)
{
	if (!CBDocumentManager::WarnFileSize(fullName))
		{
		return;
		}

	const JVCSType type = JGetVCSType(fullName);
	if (type == kJCVSType)
		{
		ViewCVSDiffs(fullName, silent);
		}
	else if (type == kJSVNType)
		{
		ViewSVNDiffs(fullName, silent);
		}
	else if (type == kJGitType)
		{
		ViewGitDiffs(fullName, silent);
		}
}

/******************************************************************************
 ViewCVSDiffs (private)

 ******************************************************************************/

void
CBDiffFileDialog::ViewCVSDiffs
	(
	const JString&	fullName,
	const bool	silent
	)
{
	itsTabGroup->ShowTab(kCVSDiffTabIndex);

	JString getCmd, diffCmd, name1, name2;
	if (!BuildCVSDiffCmd(fullName, kCurrentRevCmd, JString::empty,
						 kCurrentRevCmd, JString::empty,
						 &getCmd, &diffCmd, &name1, &name2, silent))
		{
		return;
		}

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

/******************************************************************************
 ViewCVSDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewCVSDiffs
	(
	const JString&	fullName,
	const JString&	rev1,
	const JString&	rev2,
	const bool	silent
	)
{
	if (!CBDocumentManager::WarnFileSize(fullName))
		{
		return;
		}

	JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
	if (rev1.IsEmpty())
		{
		rev1Cmd = rev2Cmd = kCurrentRevCmd;
		}
	else if (rev2.IsEmpty())
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

/******************************************************************************
 BuildCVSDiffCmd (private)

 ******************************************************************************/

bool
CBDiffFileDialog::BuildCVSDiffCmd
	(
	const JString&	fullName,
	const JIndex	rev1Cmd,
	const JString&	origRev1,
	const JIndex	rev2Cmd,
	const JString&	origRev2,
	JString*		getCmd,
	JString*		diffCmd,
	JString*		name1,
	JString*		name2,
	const bool	silent
	)
{
	JString path, name, cvsRoot;
	JSplitPathAndName(fullName, &path, &name);

	cvsRoot = JCombinePathAndName(path, JString("CVS", JString::kNoCopy));
	cvsRoot = JCombinePathAndName(cvsRoot, JString("Root", JString::kNoCopy));
	JReadFile(cvsRoot, &cvsRoot);
	cvsRoot.TrimWhitespace();

	*name1  = name;
	*name1 += ": ";

	*getCmd  = "cvs -d " + JPrepArgForExec(cvsRoot) + " get ";
	*diffCmd = "cvs -f diff ";
	if ((rev1Cmd == kPreviousRevCmd && !origRev1.IsEmpty()) ||
		(!cbIsFixedRevCmd(rev1Cmd) && !origRev1.IsEmpty()))
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
				JGetUserNotification()->ReportError(JGetString("VCSNoPreviousRevision::CBDiffFileDialog"));
				}
			return false;
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

		if (cmd2 != kCurrentRevCmd && !rev2.IsEmpty())
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

	return true;
}

/******************************************************************************
 GetCurrentCVSRevision (private)

 ******************************************************************************/

bool
CBDiffFileDialog::GetCurrentCVSRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	JString path, name, data, pattern;
	JSplitPathAndName(fullName, &path, &name);
	pattern = "^[^/]*/" + JRegex::BackslashForLiteral(name) + "/([0-9.]+)/";
	name    = JCombinePathAndName(path, JString("CVS", JString::kNoCopy));
	name    = JCombinePathAndName(name, JString("Entries", JString::kNoCopy));
	JReadFile(name, &data);

	JRegex r(pattern);
	const JStringMatch m = r.Match(data, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		*rev = m.GetSubstring(1);
		return true;
		}
	else
		{
		rev->Clear();
		return false;
		}
}

/******************************************************************************
 GetPreviousCVSRevision (private)

 ******************************************************************************/

bool
CBDiffFileDialog::GetPreviousCVSRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	if (!GetCurrentCVSRevision(fullName, rev))
		{
		return false;
		}

	JStringIterator iter(rev, kJIteratorStartAtEnd);
	iter.BeginMatch();
	if (!iter.Prev("."))
		{
		return false;
		}

	const JStringMatch& m = iter.FinishMatch();
	if (m.IsEmpty())
		{
		return false;
		}

	JUInt j;
	if (!m.GetString().ConvertToUInt(&j))
		{
		return false;
		}
	else if (j > 1)
		{
		iter.RemoveAllNext();

		*rev += JString((JUInt64) j-1);
		return true;
		}

	iter.BeginMatch();
	if (!iter.Prev("."))
		{
		return false;
		}

	iter.RemoveAllNext();
	return true;
}

/******************************************************************************
 ViewSVNDiffs (private)

 ******************************************************************************/

void
CBDiffFileDialog::ViewSVNDiffs
	(
	const JString&	fullName,
	const bool	silent
	)
{
	itsTabGroup->ShowTab(kSVNDiffTabIndex);

	JString getCmd, diffCmd, name1, name2;
	if (!BuildSVNDiffCmd(fullName, kCurrentRevCmd, JString::empty, kCurrentRevCmd, JString::empty,
						 &getCmd, &diffCmd, &name1, &name2, silent))
		{
		return;
		}

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

/******************************************************************************
 ViewSVNDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewSVNDiffs
	(
	const JString&	fullName,
	const JString&	rev1,
	const JString&	rev2,
	const bool	silent
	)
{
	if (!CBDocumentManager::WarnFileSize(fullName))
		{
		return;
		}

	JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
	if (rev1.IsEmpty())
		{
		rev1Cmd = rev2Cmd = kCurrentRevCmd;
		}
	else if (rev2.IsEmpty())
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

/******************************************************************************
 BuildSVNDiffCmd (private)

 ******************************************************************************/

bool
CBDiffFileDialog::BuildSVNDiffCmd
	(
	const JString&	fullName,
	const JIndex	rev1Cmd,
	const JString&	origRev1,
	const JIndex	rev2Cmd,
	const JString&	origRev2,
	JString*		getCmd,
	JString*		diffCmd,
	JString*		name1,
	JString*		name2,
	const bool	silent,
	const bool	forDirectory
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
	if ((rev1Cmd == kPreviousRevCmd && !origRev1.IsEmpty()) ||		// PREV from dialog
		(rev1Cmd == kTrunkCmd && !origRev1.IsEmpty()) ||			// TRUNK from dialog
		(!cbIsSVNFixedRevCmd(rev1Cmd) && !origRev1.IsEmpty()))
		{
		JIndex cmd1    = rev1Cmd;
		JString rev1   = origRev1;
		JString getRev = origRev1;
		JString revName;
		if (cmd1 == kPreviousRevCmd)
			{
			cmd1    = kRevisionNumberCmd;
			rev1    = "PREV";
			getRev  = "PREV";
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
				return false;
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
				return false;
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

		if (cmd2 != kCurrentRevCmd && !rev2.IsEmpty())
			{
			*diffCmd += ":";
			*diffCmd += JPrepArgForExec(rev2);
			}
		}
	else
		{
		*name1  += "current";
		*name2   = "edited";
		*getCmd += "-r BASE ";
		*getCmd += JPrepArgForExec(file1);
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

	return true;
}

/******************************************************************************
 BuildSVNRepositoryPath (private)

 ******************************************************************************/

static const JRegex theBranchPattern = "(?<=/)(trunk($|(?=/))|(branches|tags)/[^/]+(?=/))";

bool
CBDiffFileDialog::BuildSVNRepositoryPath
	(
	JString*		fullName,
	const JIndex	cmd,
	const JString&	rev,
	JString*		name,
	const bool	silent
	)
{
	JString repoPath;
	if (!JGetVCSRepositoryPath(*fullName, &repoPath))
		{
		if (!silent)
			{
			JGetUserNotification()->ReportError(JGetString("SVNNoRepository::CBDiffFileDialog"));
			}
		return false;
		}

	JStringIterator iter(&repoPath);
	if (!iter.Next(theBranchPattern))
		{
		if (!silent)
			{
			JGetUserNotification()->ReportError(JGetString("SVNNonstandardRepository::CBDiffFileDialog"));
			}
		return false;
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

	iter.ReplaceLastMatch(*name);
	*fullName = repoPath;
	return true;
}

/******************************************************************************
 ViewGitDiffs (private)

 ******************************************************************************/

void
CBDiffFileDialog::ViewGitDiffs
	(
	const JString&	fullName,
	const bool	silent
	)
{
	itsTabGroup->ShowTab(kGitDiffTabIndex);

	JString get1Cmd, get2Cmd, diffCmd, name1, name2;
	if (!BuildGitDiffCmd(fullName, kCurrentRevCmd, JString::empty, kCurrentRevCmd, JString::empty,
						 &get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, silent))
		{
		return;
		}

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

/******************************************************************************
 ViewGitDiffs

 ******************************************************************************/

void
CBDiffFileDialog::ViewGitDiffs
	(
	const JString&	fullName,
	const JString&	rev1,
	const JString&	rev2,
	const bool	silent
	)
{
	if (!CBDocumentManager::WarnFileSize(fullName))
		{
		return;
		}

	JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
	if (rev1.IsEmpty())
		{
		rev1Cmd = kCurrentRevCmd;
		rev2Cmd = kCurrentRevCmd;
		}
	else if (rev2.IsEmpty())
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

/******************************************************************************
 BuildGitDiffCmd (private)

 ******************************************************************************/

bool
CBDiffFileDialog::BuildGitDiffCmd
	(
	const JString&	fullName,
	const JIndex	rev1Cmd,
	const JString&	rev1,
	const JIndex	rev2Cmd,
	const JString&	rev2,
	JString*		get1Cmd,
	JString*		get2Cmd,
	JString*		diffCmd,
	JString*		name1,
	JString*		name2,
	const bool	silent
	)
{
	JString trueName;
	if (!JGetTrueName(fullName, &trueName))
		{
		return false;
		}

	JString path, name;
	JSplitPathAndName(trueName, &path, &name);

	// git show must be run with path relative to repo root

	JString gitRoot;
	if (!JSearchGitRoot(path, &gitRoot))
		{
		return false;
		}

	JString gitFile = JConvertToRelativePath(trueName, gitRoot);
	if (gitFile.BeginsWith("." ACE_DIRECTORY_SEPARATOR_STR))
		{
		JStringIterator iter(&gitFile);
		iter.RemoveNext(2);		// ACE_DIRECTORY_SEPARATOR_CHAR guarantees length 1
		}

	*name1  = name;
	*name1 += ": ";

	get1Cmd->Clear();
	get2Cmd->Clear();
	if ((rev1Cmd == kPreviousRevCmd && !rev1.IsEmpty()) ||		// PREV from dialog
		(!cbIsFixedRevCmd(rev1Cmd) && !rev1.IsEmpty()))
		{
		JString get1Rev;
		if (rev1Cmd == kPreviousRevCmd)
			{
			if (!GetPreviousGitRevision(trueName, &get1Rev))
				{
				if (!silent)
					{
					JGetUserNotification()->ReportError(JGetString("VCSNoPreviousRevision::CBDiffFileDialog"));
					}
				return false;
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
			if (!GetBestCommonGitAncestor(path, &get1Rev, get2Rev.GetBytes()))
				{
				if (!silent)
					{
					JGetUserNotification()->ReportError(JGetString("GitNoCommonAncestor::CBDiffFileDialog"));
					}
				return false;
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
	return true;
}

/******************************************************************************
 BuildGitDiffDirectoryCmd (private)

 ******************************************************************************/

bool
CBDiffFileDialog::BuildGitDiffDirectoryCmd
	(
	const JString&	path,
	const JIndex	rev1Cmd,
	const JString&	rev1,
	const JIndex	rev2Cmd,
	const JString&	rev2,
	JString*		diffCmd
	)
{
	JString trueName;
	if (!JGetTrueName(path, &trueName))
		{
		return false;
		}

	// git show must be run with path relative to repo root

	JString gitRoot;
	if (!JSearchGitRoot(path, &gitRoot))
		{
		return false;
		}

	*diffCmd = "git diff ";
	if (itsIgnoreSpaceChangeCB->IsChecked())
		{
		*diffCmd += "--ignore-space-change ";
		}

	if ((rev1Cmd == kPreviousRevCmd && !rev1.IsEmpty()) ||		// PREV from dialog
		(!cbIsFixedRevCmd(rev1Cmd) && !rev1.IsEmpty()))
		{
		JString get1Rev;
		if (rev1Cmd == kPreviousRevCmd)
			{
			const JString s = JCombinePathAndName(trueName, kDot) + kDot;
			if (!GetPreviousGitRevision(s, &get1Rev))
				{
				JGetUserNotification()->ReportError(JGetString("VCSNoPreviousRevision::CBDiffFileDialog"));
				return false;
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
			const JString s = JCombinePathAndName(trueName, kDot) + kDot;
			if (!GetCurrentGitRevision(s, &get1Rev))
				{
				JGetUserNotification()->ReportError(JGetString("VCSNoCurrentRevision::CBDiffFileDialog"));
				return false;
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

	return true;
}

/******************************************************************************
 GetCurrentGitRevision (private)

 ******************************************************************************/

bool
CBDiffFileDialog::GetCurrentGitRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	JString s;
	return GetLatestGitRevisions(fullName, rev, &s);
}

/******************************************************************************
 GetPreviousGitRevision (private)

 ******************************************************************************/

bool
CBDiffFileDialog::GetPreviousGitRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	JString s;
	return GetLatestGitRevisions(fullName, &s, rev);
}

/******************************************************************************
 GetLatestGitRevisions (private)

 ******************************************************************************/

bool
CBDiffFileDialog::GetLatestGitRevisions
	(
	const JString&	fullName,
	JString*		rev1,
	JString*		rev2
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JUtf8Byte* args[] =
		{ "git", "log", "-2", "--format=oneline", name.GetBytes(), nullptr };

	JProcess* p;
	int fromFD;
	const JError err = JProcess::Create(&p, path, args, sizeof(args),
										kJIgnoreConnection, nullptr,
										kJCreatePipe, &fromFD);
	if (err.OK())
		{
		*rev1 = JReadUntil(fromFD, ' ');	// current commit
		JIgnoreUntil(fromFD, '\n');			// ignore comment
		*rev2 = JReadUntil(fromFD, ' ');	// previous commit
		close(fromFD);
		return true;
		}
	else
		{
		err.ReportIfError();
		rev1->Clear();
		rev2->Clear();
		return false;
		}
}

/******************************************************************************
 GetBestCommonGitAncestor (private)

 ******************************************************************************/

bool
CBDiffFileDialog::GetBestCommonGitAncestor
	(
	const JString&		path,
	JString*			rev1,
	const JUtf8Byte*	rev2
	)
{
	if (JString::IsEmpty(rev2))
		{
		rev2 = "HEAD";
		}

	const JUtf8Byte* args[] =
		{ "git", "merge-base", rev1->GetBytes(), rev2, nullptr };

	JProcess* p;
	int fromFD;
	const JError err = JProcess::Create(&p, path, args, sizeof(args),
										kJIgnoreConnection, nullptr,
										kJCreatePipe, &fromFD);
	if (err.OK())
		{
		if (!JReadAll(fromFD, rev1))
			{
			return false;
			}
		close(fromFD);
		rev1->TrimWhitespace();
		return true;
		}
	else
		{
		err.ReportIfError();
		rev1->Clear();
		return false;
		}
}

/******************************************************************************
 GetSmartDiffItemText

 ******************************************************************************/

const JString&
CBDiffFileDialog::GetSmartDiffItemText
	(
	const bool	onDisk,
	const JString&	fullName,
	bool*		enable
	)
	const
{
	if (!onDisk)
		{
		*enable = false;
		return JGetString("DiffBackupItemText::CBDiffFileDialog");
		}

	JString path, origFileName;
	JSplitPathAndName(fullName, &path, &origFileName);

	bool isSafetySave, isBackup;
	JString fileName = GetSmartDiffInfo(origFileName, &isSafetySave, &isBackup);

	const JUtf8Byte* id = nullptr;
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
		backupName.Append("~");
		*enable = JFileReadable(backupName);
		id      = "DiffBackupItemText::CBDiffFileDialog";
		}
	assert( id != nullptr );
	return JGetString(id);
}

/******************************************************************************
 ViewDiffs

	We require onDisk to avoid being passed an invalid fullName.

 ******************************************************************************/

void
CBDiffFileDialog::ViewDiffs
	(
	const bool	onDisk,
	const JString&	fullName,
	const bool	silent
	)
{
	if (!onDisk)
		{
		return;
		}

	JString path, origFileName;
	JSplitPathAndName(fullName, &path, &origFileName);

	bool isSafetySave, isBackup;
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
		backupName.Append("~");
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
	const JString&	origFileName,
	bool*		isSafetySave,
	bool*		isBackup
	)
	const
{
	JString fileName = origFileName;

	JStringIterator iter(&fileName);

	*isSafetySave = false;
	while (fileName.BeginsWith("#") && fileName.EndsWith("#"))
		{
		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		iter.RemoveNext();

		iter.MoveTo(kJIteratorStartAtEnd, 0);
		iter.RemovePrev();

		*isSafetySave = true;
		}

	iter.MoveTo(kJIteratorStartAtEnd, 0);

	*isBackup = false;
	while (!(*isSafetySave) && fileName.EndsWith("~"))		// don't strip if safety save
		{
		iter.RemovePrev();
		*isBackup = true;
		}

	return fileName;
}

/******************************************************************************
 CheckFile (private)

 ******************************************************************************/

bool
CBDiffFileDialog::CheckFile
	(
	JXFileInput* widget
	)
	const
{
	widget->ShouldAllowInvalidFile(false);
	const bool valid = widget->InputValid();
	widget->ShouldAllowInvalidFile(true);
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
	if (JGetChooseSaveFile()->ChooseFile(JString::empty, JString::empty, origName, &newName))
		{
		widget->Focus();
		widget->GetText()->SetText(newName);
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
	if (JGetChooseSaveFile()->ChooseRPath(JString::empty, JString::empty, origPath, &newPath))
		{
		widget->Focus();
		widget->GetText()->SetText(newPath);
		}
}

/******************************************************************************
 CheckVCSFileOrPath (private)

 ******************************************************************************/

bool
CBDiffFileDialog::CheckVCSFileOrPath
	(
	JXFileInput*	widget,
	const bool		reportError,
	JString*		fullName		// can be nullptr
	)
	const
{
	const JString& text = widget->GetText()->GetText();
	if (!text.IsEmpty())
		{
		JString basePath, s;
		widget->GetBasePath(&basePath);
		if (JConvertToAbsolutePath(text, basePath, &s))
			{
			if (fullName != nullptr)
				{
				*fullName = s;
				}
			return true;
			}
		}

	if (reportError)
		{
		widget->Focus();
		JGetUserNotification()->ReportError(JGetString("VCSDiffTargetInvalid::CBDiffFileDialog"));
		}

	if (fullName != nullptr)
		{
		fullName->Clear();
		}
	return false;
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

bool
CBDiffFileDialog::CheckSVNFileOrPath
	(
	JXFileInput*	widget,
	const bool		reportError,
	JString*		fullName		// can be nullptr
	)
	const
{
	const JString& text = widget->GetText()->GetText();
	if (!text.IsEmpty())
		{
		if (JIsURL(text))
			{
			if (fullName != nullptr)
				{
				*fullName = text;
				}
			return true;
			}

		JString basePath, s;
		widget->GetBasePath(&basePath);
		if (JConvertToAbsolutePath(text, basePath, &s))
			{
			if (fullName != nullptr)
				{
				*fullName = s;
				}
			return true;
			}
		}

	if (reportError)
		{
		widget->Focus();
		JGetUserNotification()->ReportError(JGetString("SVNDiffTargetInvalid::CBDiffFileDialog"));
		}

	if (fullName != nullptr)
		{
		fullName->Clear();
		}
	return false;
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
	if (CBGetDocumentManager()->GetActiveProjectDocument(&doc))
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
		bool ignoreSpaceChange, ignoreBlankLines;
		input >> JBoolFromString(ignoreSpaceChange)
			  >> JBoolFromString(ignoreBlankLines);
		itsIgnoreSpaceChangeCB->SetState(ignoreSpaceChange);
		itsIgnoreBlankLinesCB->SetState(ignoreBlankLines);
		}

	if (vers >= 1)
		{
		bool stayOpen;
		input >> JBoolFromString(stayOpen);
		itsStayOpenCB->SetState(stayOpen);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBDiffFileDialog::WritePrefs
	(
	std::ostream& output
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

	output << ' ' << JBoolToString(itsIgnoreSpaceChangeCB->IsChecked())
				  << JBoolToString(itsIgnoreBlankLinesCB->IsChecked())
				  << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ';
}
