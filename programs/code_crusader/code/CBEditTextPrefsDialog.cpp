/******************************************************************************
 CBEditTextPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "CBEditTextPrefsDialog.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "CBSampleText.h"
#include "CBSearchTextDialog.h"
#include "CBFnMenuUpdater.h"
#include "CBEmulator.h"
#include "cbmUtil.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXIntegerInput.h>
#include <JXCharInput.h>
#include <JXStaticText.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXChooseMonoFont.h>
#include <JXScrollbar.h>
#include <JXChooseColorDialog.h>
#include <JXColorManager.h>
#include <JXHelpManager.h>
#include <JFontManager.h>
#include <jAssert.h>

// emulators

static const JUtf8Byte* kEmulatorMenuStr =
	"  None"
	"| vi";

enum
{
	kNoEmulatorCmd = 1,
	kVIEmulatorCmd
};

static const CBEmulator kMenuIndexToEmulator[] =
{
	kCBNoEmulator,
	kCBVIEmulator
};

static const JIndex kEmulatorToMenuIndex[] =
{
	kNoEmulatorCmd,
	kVIEmulatorCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditTextPrefsDialog::CBEditTextPrefsDialog
	(
	CBTextDocument* doc
	)
	:
	JXDialogDirector(CBGetApplication(), true)
{
	itsDoc               = doc;
	itsChooseColorDialog = nullptr;

	itsOrigEmulatorIndex =
		itsEmulatorIndex = kEmulatorToMenuIndex[
			CBGetPrefsManager()->GetEmulator() ];

	BuildWindow(doc);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditTextPrefsDialog::~CBEditTextPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditTextPrefsDialog::BuildWindow
	(
	CBTextDocument* doc
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 640,530, JString::empty);
	assert( window != nullptr );

	itsTabCharCountInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,130, 40,20);
	assert( itsTabCharCountInput != nullptr );

	itsCRMLineWidthInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,150, 40,20);
	assert( itsCRMLineWidthInput != nullptr );

	itsUndoDepthInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,170, 40,20);
	assert( itsUndoDepthInput != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,490, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,490, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBEditTextPrefsDialog::shortcuts::JXLayout"));

	itsCreateBackupCB =
		jnew JXTextCheckbox(JGetString("itsCreateBackupCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,230, 300,20);
	assert( itsCreateBackupCB != nullptr );

	itsAutoIndentCB =
		jnew JXTextCheckbox(JGetString("itsAutoIndentCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,160, 280,20);
	assert( itsAutoIndentCB != nullptr );

	auto* spacesPerTabLabel =
		jnew JXStaticText(JGetString("spacesPerTabLabel::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,130, 210,20);
	assert( spacesPerTabLabel != nullptr );
	spacesPerTabLabel->SetToLabel();

	itsExtraSpaceWindTitleCB =
		jnew JXTextCheckbox(JGetString("itsExtraSpaceWindTitleCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,310, 300,20);
	assert( itsExtraSpaceWindTitleCB != nullptr );

	itsOpenComplFileOnTopCB =
		jnew JXTextCheckbox(JGetString("itsOpenComplFileOnTopCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,280, 300,20);
	assert( itsOpenComplFileOnTopCB != nullptr );

	itsFontMenu =
		jnew JXChooseMonoFont(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 280,60);
	assert( itsFontMenu != nullptr );

	itsEmulatorMenu =
		jnew JXTextMenu(JGetString("itsEmulatorMenu::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 280,25);
	assert( itsEmulatorMenu != nullptr );

	auto* undoDepthLabel =
		jnew JXStaticText(JGetString("undoDepthLabel::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,170, 210,20);
	assert( undoDepthLabel != nullptr );
	undoDepthLabel->SetToLabel();

	itsUseDNDCB =
		jnew JXTextCheckbox(JGetString("itsUseDNDCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,130, 280,20);
	assert( itsUseDNDCB != nullptr );

	itsOnlyBackupIfNoneCB =
		jnew JXTextCheckbox(JGetString("itsOnlyBackupIfNoneCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,250, 300,20);
	assert( itsOnlyBackupIfNoneCB != nullptr );

	itsLeftToFrontOfTextCB =
		jnew JXTextCheckbox(JGetString("itsLeftToFrontOfTextCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,450, 300,20);
	assert( itsLeftToFrontOfTextCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,490, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditTextPrefsDialog::shortcuts::JXLayout"));

	itsBalanceWhileTypingCB =
		jnew JXTextCheckbox(JGetString("itsBalanceWhileTypingCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,310, 280,20);
	assert( itsBalanceWhileTypingCB != nullptr );

	itsScrollToBalanceCB =
		jnew JXTextCheckbox(JGetString("itsScrollToBalanceCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,330, 280,20);
	assert( itsScrollToBalanceCB != nullptr );

	itsBeepWhenTypeUnbalancedCB =
		jnew JXTextCheckbox(JGetString("itsBeepWhenTypeUnbalancedCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,350, 280,20);
	assert( itsBeepWhenTypeUnbalancedCB != nullptr );

	auto* cpmLabel =
		jnew JXStaticText(JGetString("cpmLabel::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,150, 210,20);
	assert( cpmLabel != nullptr );
	cpmLabel->SetToLabel();

	itsSmartTabCB =
		jnew JXTextCheckbox(JGetString("itsSmartTabCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,220, 280,20);
	assert( itsSmartTabCB != nullptr );

	itsPWModRG =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 460,350, 160,80);
	assert( itsPWModRG != nullptr );

	auto* ctrlMetaLabel =
		jnew JXTextRadioButton(JXTEBase::kCtrlMetaPWMod, JGetString("ctrlMetaLabel::CBEditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 90,20);
	assert( ctrlMetaLabel != nullptr );

	auto* mod2Label =
		jnew JXTextRadioButton(JXTEBase::kMod2PWMod, JGetString("mod2Label::CBEditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,30, 70,20);
	assert( mod2Label != nullptr );

	auto* mod3Label =
		jnew JXTextRadioButton(JXTEBase::kMod3PWMod, JGetString("mod3Label::CBEditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 70,20);
	assert( mod3Label != nullptr );

	auto* mod4Label =
		jnew JXTextRadioButton(JXTEBase::kMod4PWMod, JGetString("mod4Label::CBEditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,30, 60,20);
	assert( mod4Label != nullptr );

	auto* mod5Label =
		jnew JXTextRadioButton(JXTEBase::kMod5PWMod, JGetString("mod5Label::CBEditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,50, 60,20);
	assert( mod5Label != nullptr );

	auto* partialWordTitle =
		jnew JXStaticText(JGetString("partialWordTitle::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,360, 120,60);
	assert( partialWordTitle != nullptr );

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,250, 280,20);
	assert( itsCopyWhenSelectCB != nullptr );

	itsColorButton[CBPrefsManager::kTextColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[CBPrefsManager::kTextColorIndex-1]::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 400,50, 40,20);
	assert( itsColorButton[CBPrefsManager::kTextColorIndex-1] != nullptr );

	itsColorButton[CBPrefsManager::kBackColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[CBPrefsManager::kBackColorIndex-1]::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 440,50, 80,20);
	assert( itsColorButton[CBPrefsManager::kBackColorIndex-1] != nullptr );

	auto* changeColorLabel =
		jnew JXStaticText(JGetString("changeColorLabel::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,50, 90,20);
	assert( changeColorLabel != nullptr );
	changeColorLabel->SetToLabel();

	itsColorButton[CBPrefsManager::kSelColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[CBPrefsManager::kSelColorIndex-1]::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 560,50, 60,20);
	assert( itsColorButton[CBPrefsManager::kSelColorIndex-1] != nullptr );

	itsColorButton[CBPrefsManager::kSelLineColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[CBPrefsManager::kSelLineColorIndex-1]::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 400,70, 50,20);
	assert( itsColorButton[CBPrefsManager::kSelLineColorIndex-1] != nullptr );

	itsColorButton[CBPrefsManager::kCaretColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[CBPrefsManager::kCaretColorIndex-1]::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 520,50, 40,20);
	assert( itsColorButton[CBPrefsManager::kCaretColorIndex-1] != nullptr );

	itsDefColorsButton =
		jnew JXTextButton(JGetString("itsDefColorsButton::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 400,95, 100,20);
	assert( itsDefColorsButton != nullptr );

	itsInvColorsButton =
		jnew JXTextButton(JGetString("itsInvColorsButton::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 500,95, 100,20);
	assert( itsInvColorsButton != nullptr );

	auto* colorSetsLabel =
		jnew JXStaticText(JGetString("colorSetsLabel::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,95, 90,20);
	assert( colorSetsLabel != nullptr );
	colorSetsLabel->SetToLabel();

	itsTabToSpacesCB =
		jnew JXTextCheckbox(JGetString("itsTabToSpacesCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,190, 280,20);
	assert( itsTabToSpacesCB != nullptr );

	itsHomeEndCB =
		jnew JXTextCheckbox(JGetString("itsHomeEndCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 330,470, 300,20);
	assert( itsHomeEndCB != nullptr );

	itsScrollCaretCB =
		jnew JXTextCheckbox(JGetString("itsScrollCaretCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 330,490, 300,20);
	assert( itsScrollCaretCB != nullptr );

	itsSortFnMenuCB =
		jnew JXTextCheckbox(JGetString("itsSortFnMenuCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,390, 280,20);
	assert( itsSortFnMenuCB != nullptr );

	itsPackFnMenuCB =
		jnew JXTextCheckbox(JGetString("itsPackFnMenuCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,430, 280,20);
	assert( itsPackFnMenuCB != nullptr );

	itsColorButton[CBPrefsManager::kRightMarginColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[CBPrefsManager::kRightMarginColorIndex-1]::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 450,70, 160,20);
	assert( itsColorButton[CBPrefsManager::kRightMarginColorIndex-1] != nullptr );

	itsRightMarginCB =
		jnew JXTextCheckbox(JGetString("itsRightMarginCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,190, 210,20);
	assert( itsRightMarginCB != nullptr );

	itsRightMarginInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,190, 40,20);
	assert( itsRightMarginInput != nullptr );

	itsSampleText =
		jnew CBSampleText(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,20, 310,25);
	assert( itsSampleText != nullptr );

	itsNSInFnMenuCB =
		jnew JXTextCheckbox(JGetString("itsNSInFnMenuCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,410, 280,20);
	assert( itsNSInFnMenuCB != nullptr );

	auto* wordWrapHint =
		jnew JXStaticText(JGetString("wordWrapHint::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,460, 250,20);
	assert( wordWrapHint != nullptr );
	wordWrapHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	wordWrapHint->SetToLabel();

	itsMiddleButtonPasteCB =
		jnew JXTextCheckbox(JGetString("itsMiddleButtonPasteCB::CBEditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,270, 280,20);
	assert( itsMiddleButtonPasteCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditTextPrefsDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsEmulatorMenu->SetMenuItems(kEmulatorMenuStr);
	itsEmulatorMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsEmulatorMenu->SetToPopupChoice(true, itsEmulatorIndex);
	ListenTo(itsEmulatorMenu);

	itsCreateBackupCB->SetState(doc->WillMakeBackupFile());
	itsOnlyBackupIfNoneCB->SetState(!doc->WillMakeNewBackupEveryOpen());
	itsExtraSpaceWindTitleCB->SetState(doc->WillAllocateTitleSpace());
	itsOpenComplFileOnTopCB->SetState(doc->WillOpenComplFileOnTop());

	CBTextEditor* te = doc->GetTextEditor();

	itsAutoIndentCB->SetState(te->GetText()->WillAutoIndent());
	itsUseDNDCB->SetState(te->CBAllowsDragAndDrop());
	itsLeftToFrontOfTextCB->SetState(te->WillMoveToFrontOfText());

	itsBalanceWhileTypingCB->SetState(te->WillBalanceWhileTyping());
	itsScrollToBalanceCB->SetState(te->WillScrollToBalance());
	itsBeepWhenTypeUnbalancedCB->SetState(te->WillBeepWhenTypeUnbalanced());

	CBFnMenuUpdater* updater = CBGetFnMenuUpdater();
	itsSortFnMenuCB->SetState(updater->WillSortFnNames());
	itsNSInFnMenuCB->SetState(updater->WillIncludeNamespace());
	itsPackFnMenuCB->SetState(updater->WillPackFnNames());

	itsSmartTabCB->SetState(te->TabIsSmart());
	itsTabToSpacesCB->SetState(te->GetText()->TabInsertsSpaces());
	itsCopyWhenSelectCB->SetState(te->WillCopyWhenSelect());
	itsMiddleButtonPasteCB->SetState(te->MiddleButtonWillPaste());

	const JFont& font = te->GetText()->GetDefaultFont();
	itsFontMenu->SetFont(font.GetName(), font.GetSize());

	itsTabCharCountInput->SetValue(te->GetTabCharCount());
	itsTabCharCountInput->SetLowerLimit(1);

	itsCRMLineWidthInput->SetValue(te->GetText()->GetCRMLineWidth());
	itsCRMLineWidthInput->SetLowerLimit(1);

	itsUndoDepthInput->SetValue(te->GetText()->GetUndoDepth());
	itsUndoDepthInput->SetLowerLimit(1);

	JSize margin;
	itsRightMarginCB->SetState(te->GetRightMarginWidth(&margin));
	itsRightMarginInput->SetValue(margin);
	itsRightMarginInput->SetLowerLimit(1);

	itsPWModRG->SelectItem(te->GetPartialWordModifier());
	itsHomeEndCB->SetState(JXTEBase::WillUseWindowsHomeEnd());
	itsScrollCaretCB->SetState(CBTextEditor::CaretWillFollowScroll());

	ListenTo(itsCreateBackupCB);
	ListenTo(itsBalanceWhileTypingCB);
	UpdateDisplay();

	// must do this after SetWindow()

	CBPrefsManager* prefsMgr = CBGetPrefsManager();
	for (JUnsignedOffset i=0; i<CBPrefsManager::kColorCount; i++)
		{
		itsColor[i] = prefsMgr->GetColor(i+1);
		ListenTo(itsColorButton[i]);
		}

	ListenTo(itsDefColorsButton);
	ListenTo(itsInvColorsButton);
	ListenTo(itsFontMenu);
	ListenTo(itsRightMarginCB);

	UpdateSampleText();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditTextPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateSettings();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		JXGetHelpManager()->ShowSection("CBEditorHelp-Prefs");
		}

	else if ((sender == itsCreateBackupCB || sender == itsBalanceWhileTypingCB) &&
			 message.Is(JXCheckbox::kPushed))
		{
		UpdateDisplay();
		}

	else if (sender == itsFontMenu && message.Is(JXChooseMonoFont::kFontChanged))
		{
		UpdateSampleText();
		}
	else if (sender == itsRightMarginCB && message.Is(JXCheckbox::kPushed))
		{
		UpdateSampleText();
		}

	else if (message.Is(JXButton::kPushed) && HandleColorButton(sender))
		{
		// function did all the work
		}

	else if (sender == itsChooseColorDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			ChangeColor(itsChooseColorIndex, itsChooseColorDialog->GetColor());
			}
		itsChooseColorDialog = nullptr;
		}

	else if (sender == itsDefColorsButton && message.Is(JXButton::kPushed))
		{
		SetDefaultColors();
		}
	else if (sender == itsInvColorsButton && message.Is(JXButton::kPushed))
		{
		SetReverseVideoColors();
		}

	else if (sender == itsEmulatorMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* info =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( info != nullptr );
		itsEmulatorIndex = info->GetIndex();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateSettings (private)

 ******************************************************************************/

void
CBEditTextPrefsDialog::UpdateSettings()
{
	CBTextEditor* te = itsDoc->GetTextEditor();

	JString fontName;
	JSize fontSize;
	itsFontMenu->GetFont(&fontName, &fontSize);
	const bool fontChanged = fontName != te->GetText()->GetDefaultFont().GetName() ||
		fontSize != te->GetText()->GetDefaultFont().GetSize();

	JFontManager* fontMgr = GetDisplay()->GetFontManager();

	JFloat vScrollScale = 1.0;
	if (fontChanged)
		{
		const JFloat h1 = te->GetText()->GetDefaultFont().GetLineHeight(fontMgr);
		const JFloat h2 = JFontManager::GetFont(fontName, fontSize).GetLineHeight(fontMgr);
		vScrollScale    = h2 / h1;
		}

	JInteger tabCharCount;
	bool ok = itsTabCharCountInput->GetValue(&tabCharCount);
	assert( ok );

	JInteger crmLineWidth;
	ok = itsCRMLineWidthInput->GetValue(&crmLineWidth);
	assert( ok );

	JInteger undoDepth;
	ok = itsUndoDepthInput->GetValue(&undoDepth);
	assert( ok );

	JInteger rightMargin;
	ok = itsRightMarginInput->GetValue(&rightMargin);
	assert( ok );

	CBPrefsManager* prefsMgr = CBGetPrefsManager();
	const bool textColorChanged =
		itsColor[ CBPrefsManager::kTextColorIndex-1 ] !=
		prefsMgr->GetColor(CBPrefsManager::kTextColorIndex);

	// set colors before RecalcStyles() so stylers update themselves

	prefsMgr->SetDefaultFont(fontName, fontSize);
	for (JIndex j=1; j<=CBPrefsManager::kColorCount; j++)
		{
		prefsMgr->SetColor(j, itsColor[j-1]);
		}

	JPtrArray<CBTextDocument>* docList = CBGetDocumentManager()->GetTextDocList();
	const JSize docCount = docList->GetElementCount();

	JProgressDisplay* pg = JNewPG();
	pg->FixedLengthProcessBeginning(docCount, JGetString("UpdatingPrefs::CBEditTextPrefsDialog"), false, false);

	for (JIndex i=1; i<=docCount; i++)
		{
		CBTextDocument* doc = docList->GetElement(i);

		doc->ShouldMakeBackupFile(itsCreateBackupCB->IsChecked());
		doc->ShouldMakeNewBackupEveryOpen(!itsOnlyBackupIfNoneCB->IsChecked());
		doc->ShouldAllocateTitleSpace(itsExtraSpaceWindTitleCB->IsChecked());
		doc->ShouldOpenComplFileOnTop(itsOpenComplFileOnTopCB->IsChecked());

		te = doc->GetTextEditor();

		if (itsEmulatorIndex != itsOrigEmulatorIndex)
			{
			JTEKeyHandler* handler;
			CBInstallEmulator(kMenuIndexToEmulator[ itsEmulatorIndex-1 ], te, &handler);
			}

		te->GetText()->ShouldAutoIndent(itsAutoIndentCB->IsChecked());
		te->CBShouldAllowDragAndDrop(itsUseDNDCB->IsChecked());
		te->ShouldMoveToFrontOfText(itsLeftToFrontOfTextCB->IsChecked());

		te->ShouldBalanceWhileTyping(itsBalanceWhileTypingCB->IsChecked());
		te->ShouldScrollToBalance(itsScrollToBalanceCB->IsChecked());
		te->ShouldBeepWhenTypeUnbalanced(itsBeepWhenTypeUnbalancedCB->IsChecked());

		te->TabShouldBeSmart(itsSmartTabCB->IsChecked());
		te->GetText()->TabShouldInsertSpaces(itsTabToSpacesCB->IsChecked());

		if (fontChanged)
			{
			JXScrollbar *hScrollbar, *vScrollbar;
			const bool ok = te->GetScrollbars(&hScrollbar, &vScrollbar);
			assert( ok );
			vScrollbar->PrepareForScaledMaxValue(vScrollScale);

			te->SetFont(fontName, fontSize, tabCharCount);
			}
		else
			{
			te->SetTabCharCount(tabCharCount);
			}

		te->GetText()->SetCRMLineWidth(crmLineWidth);
		te->GetText()->SetUndoDepth(undoDepth);
		te->SetRightMarginWidth(itsRightMarginCB->IsChecked(), rightMargin);

		te->GetText()->SetDefaultFontStyle(itsColor [ CBPrefsManager::kTextColorIndex-1 ]);
		te->SetBackColor(itsColor [ CBPrefsManager::kBackColorIndex-1 ]);
		te->SetFocusColor(itsColor [ CBPrefsManager::kBackColorIndex-1 ]);
		te->SetCaretColor(itsColor [ CBPrefsManager::kCaretColorIndex-1 ]);
		te->SetSelectionColor(itsColor [ CBPrefsManager::kSelColorIndex-1 ]);
		te->SetSelectionOutlineColor(itsColor [ CBPrefsManager::kSelLineColorIndex-1 ]);
		te->SetRightMarginColor(itsColor [ CBPrefsManager::kRightMarginColorIndex-1 ]);

		if (textColorChanged)
			{
			te->RecalcStyles();
			}

		// force update of insertion font

		JIndex caretIndex;
		if (te->GetCaretLocation(&caretIndex))
			{
			te->SetCaretLocation(caretIndex);
			}

		pg->IncrementProgress();
		}

	CBFnMenuUpdater* updater = CBGetFnMenuUpdater();
	updater->ShouldSortFnNames(itsSortFnMenuCB->IsChecked());
	updater->ShouldIncludeNamespace(itsNSInFnMenuCB->IsChecked());
	updater->ShouldPackFnNames(itsPackFnMenuCB->IsChecked());

	JXTEBase::SetPartialWordModifier(
		(JXTEBase::PartialWordModifier) itsPWModRG->GetSelectedItem());

	JXTEBase::ShouldUseWindowsHomeEnd(itsHomeEndCB->IsChecked());
	CBTextEditor::CaretShouldFollowScroll(itsScrollCaretCB->IsChecked());
	JTextEditor::ShouldCopyWhenSelect(itsCopyWhenSelectCB->IsChecked());
	JXTEBase::MiddleButtonShouldPaste(itsMiddleButtonPasteCB->IsChecked());

	CBGetSearchTextDialog()->SetFont(fontMgr->GetFont(fontName, fontSize));

	itsDoc->JPrefObject::WritePrefs();

	if (itsEmulatorIndex != itsOrigEmulatorIndex)
		{
		prefsMgr->SetEmulator(kMenuIndexToEmulator[ itsEmulatorIndex-1 ]);
		}

	CBMWriteSharedPrefs(true);

	pg->ProcessFinished();
	jdelete pg;
}

/******************************************************************************
 HandleColorButton (private)

	Returns true if the sender is one of our color buttons.

 ******************************************************************************/

bool
CBEditTextPrefsDialog::HandleColorButton
	(
	JBroadcaster* sender
	)
{
	itsChooseColorIndex = 0;
	for (JUnsignedOffset i=0; i<CBPrefsManager::kColorCount; i++)
		{
		if (sender == itsColorButton[i])
			{
			itsChooseColorIndex = i+1;
			break;
			}
		}

	if (itsChooseColorIndex == 0)
		{
		return false;
		}

	assert( itsChooseColorDialog == nullptr );

	itsChooseColorDialog =
		jnew JXChooseColorDialog(this, itsColor[itsChooseColorIndex-1]);
	assert( itsChooseColorDialog != nullptr );

	ListenTo(itsChooseColorDialog);
	itsChooseColorDialog->BeginDialog();

	return true;
}

/******************************************************************************
 SetDefaultColors (private)

 ******************************************************************************/

void
CBEditTextPrefsDialog::SetDefaultColors()
{
	ChangeColor(CBPrefsManager::kTextColorIndex,        JColorManager::GetBlackColor());
	ChangeColor(CBPrefsManager::kBackColorIndex,        JColorManager::GetWhiteColor());
	ChangeColor(CBPrefsManager::kCaretColorIndex,       JColorManager::GetRedColor());
	ChangeColor(CBPrefsManager::kSelColorIndex,         JColorManager::GetDefaultSelectionColor());
	ChangeColor(CBPrefsManager::kSelLineColorIndex,     JColorManager::GetBlueColor());
	ChangeColor(CBPrefsManager::kRightMarginColorIndex, JColorManager::GetGrayColor(70));
}

/******************************************************************************
 SetReverseVideoColors (private)

 ******************************************************************************/

void
CBEditTextPrefsDialog::SetReverseVideoColors()
{
	ChangeColor(CBPrefsManager::kTextColorIndex,        JColorManager::GetWhiteColor());
	ChangeColor(CBPrefsManager::kBackColorIndex,        JColorManager::GetBlackColor());
	ChangeColor(CBPrefsManager::kCaretColorIndex,       JColorManager::GetWhiteColor());
	ChangeColor(CBPrefsManager::kSelColorIndex,         JColorManager::GetBlueColor());
	ChangeColor(CBPrefsManager::kSelLineColorIndex,     JColorManager::GetCyanColor());
	ChangeColor(CBPrefsManager::kRightMarginColorIndex, JColorManager::GetGrayColor(80));
}

/******************************************************************************
 ChangeColor (private)

 ******************************************************************************/

void
CBEditTextPrefsDialog::ChangeColor
	(
	const JIndex	colorIndex,
	const JColorID	color
	)
{
	assert( CBPrefsManager::ColorIndexValid(colorIndex) );

	itsColor [ colorIndex-1 ] = color;
	UpdateSampleText();
}

/******************************************************************************
 UpdateSampleText (private)

 ******************************************************************************/

void
CBEditTextPrefsDialog::UpdateSampleText()
{
	JString name;
	JSize size;
	itsFontMenu->GetFont(&name, &size);
	itsSampleText->SetFont(
		JFontManager::GetFont(name, size,
			JFontStyle(itsColor [ CBPrefsManager::kTextColorIndex-1 ])));

	itsSampleText->SetBackColor(itsColor [ CBPrefsManager::kBackColorIndex-1 ] );
	itsSampleText->SetFocusColor(itsColor [ CBPrefsManager::kBackColorIndex-1 ] );

	itsSampleText->SetCaretColor(itsColor [ CBPrefsManager::kCaretColorIndex-1 ] );
	itsSampleText->SetSelectionColor(itsColor [ CBPrefsManager::kSelColorIndex-1 ] );
	itsSampleText->SetSelectionOutlineColor(itsColor [ CBPrefsManager::kSelLineColorIndex-1 ] );

	itsSampleText->ShowRightMargin(itsRightMarginCB->IsChecked(),
								   itsColor [ CBPrefsManager::kRightMarginColorIndex-1 ] );
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBEditTextPrefsDialog::UpdateDisplay()
{
	if (itsCreateBackupCB->IsChecked())
		{
		itsOnlyBackupIfNoneCB->Activate();
		}
	else
		{
		itsOnlyBackupIfNoneCB->Deactivate();
		}

	if (itsBalanceWhileTypingCB->IsChecked())
		{
		itsScrollToBalanceCB->Activate();
		itsBeepWhenTypeUnbalancedCB->Activate();
		}
	else
		{
		itsScrollToBalanceCB->Deactivate();
		itsBeepWhenTypeUnbalancedCB->Deactivate();
		}
}
