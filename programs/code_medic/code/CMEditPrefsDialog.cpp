/******************************************************************************
 CMEditPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2001-10 by John Lindal.

 ******************************************************************************/

#include "CMEditPrefsDialog.h"
#include "CMDebuggerProgramInput.h"
#include "cbmUtil.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JFontManager.h>
#include <JStringIterator.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMEditPrefsDialog::CMEditPrefsDialog
	(
	JXDirector*					supervisor,
	const JString&				gdbCmd,
	const JString&				jdbCmd,
	const JString&				editFileCmd,
	const JString&				editFileLineCmd,
	const JPtrArray<JString>&	cSourceSuffixes,
	const JPtrArray<JString>&	cHeaderSuffixes,
	const JPtrArray<JString>&	javaSuffixes,
	const JPtrArray<JString>&	phpSuffixes,
	const JPtrArray<JString>&	fortranSuffixes,
	const JPtrArray<JString>&	dSuffixes,
	const JPtrArray<JString>&	goSuffixes
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow(gdbCmd, jdbCmd, editFileCmd, editFileLineCmd,
				cSourceSuffixes, cHeaderSuffixes,
				javaSuffixes, phpSuffixes, fortranSuffixes,
				dSuffixes, goSuffixes);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMEditPrefsDialog::~CMEditPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CMEditPrefsDialog::BuildWindow
	(
	const JString&				gdbCmd,
	const JString&				jdbCmd,
	const JString&				editFileCmd,
	const JString&				editFileLineCmd,
	const JPtrArray<JString>&	cSourceSuffixes,
	const JPtrArray<JString>&	cHeaderSuffixes,
	const JPtrArray<JString>&	javaSuffixes,
	const JPtrArray<JString>&	phpSuffixes,
	const JPtrArray<JString>&	fortranSuffixes,
	const JPtrArray<JString>&	dSuffixes,
	const JPtrArray<JString>&	goSuffixes
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,480, JString::empty);
	assert( window != nullptr );

	itsGDBCmd =
		jnew CMDebuggerProgramInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 350,20);
	assert( itsGDBCmd != nullptr );

	itsJDBCmd =
		jnew CMDebuggerProgramInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,110, 350,20);
	assert( itsJDBCmd != nullptr );

	itsEditFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,160, 240,20);
	assert( itsEditFileCmdInput != nullptr );

	itsEditFileLineCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,180, 240,20);
	assert( itsEditFileLineCmdInput != nullptr );

	itsCSourceSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,240, 240,20);
	assert( itsCSourceSuffixInput != nullptr );

	itsCHeaderSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,260, 240,20);
	assert( itsCHeaderSuffixInput != nullptr );

	itsJavaSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,290, 240,20);
	assert( itsJavaSuffixInput != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,450, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CMEditPrefsDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,450, 60,20);
	assert( cancelButton != nullptr );

	auto* cSourceFileLabel =
		jnew JXStaticText(JGetString("cSourceFileLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,240, 170,20);
	assert( cSourceFileLabel != nullptr );
	cSourceFileLabel->SetToLabel();

	auto* cHeaderFileLabel =
		jnew JXStaticText(JGetString("cHeaderFileLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,260, 170,20);
	assert( cHeaderFileLabel != nullptr );
	cHeaderFileLabel->SetToLabel();

	auto* cmdHint =
		jnew JXStaticText(JGetString("cmdHint::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,200, 270,20);
	assert( cmdHint != nullptr );
	cmdHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	cmdHint->SetToLabel();

	auto* editFileLabel =
		jnew JXStaticText(JGetString("editFileLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 170,20);
	assert( editFileLabel != nullptr );
	editFileLabel->SetToLabel();

	auto* editLineLabel =
		jnew JXStaticText(JGetString("editLineLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 170,20);
	assert( editLineLabel != nullptr );
	editLineLabel->SetToLabel();

	auto* javaSourceLabel =
		jnew JXStaticText(JGetString("javaSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,290, 159,20);
	assert( javaSourceLabel != nullptr );
	javaSourceLabel->SetToLabel();

	auto* fortranSourceLabel =
		jnew JXStaticText(JGetString("fortranSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,350, 170,20);
	assert( fortranSourceLabel != nullptr );
	fortranSourceLabel->SetToLabel();

	auto* gdbCmdLabel =
		jnew JXStaticText(JGetString("gdbCmdLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 340,20);
	assert( gdbCmdLabel != nullptr );
	gdbCmdLabel->SetToLabel();

	itsChooseGDBButton =
		jnew JXTextButton(JGetString("itsChooseGDBButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,40, 60,20);
	assert( itsChooseGDBButton != nullptr );

	auto* gdbScriptHint =
		jnew JXStaticText(JGetString("gdbScriptHint::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 410,20);
	assert( gdbScriptHint != nullptr );
	gdbScriptHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	gdbScriptHint->SetToLabel();

	auto* jvmCmdLabel =
		jnew JXStaticText(JGetString("jvmCmdLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 340,20);
	assert( jvmCmdLabel != nullptr );
	jvmCmdLabel->SetToLabel();

	itsChooseJDBButton =
		jnew JXTextButton(JGetString("itsChooseJDBButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,110, 60,20);
	assert( itsChooseJDBButton != nullptr );

	auto* jvmScriptHint =
		jnew JXStaticText(JGetString("jvmScriptHint::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 410,20);
	assert( jvmScriptHint != nullptr );
	jvmScriptHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	jvmScriptHint->SetToLabel();

	itsPHPSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,320, 240,20);
	assert( itsPHPSuffixInput != nullptr );

	auto* phpSourceLabel =
		jnew JXStaticText(JGetString("phpSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,320, 159,20);
	assert( phpSourceLabel != nullptr );
	phpSourceLabel->SetToLabel();

	itsFortranSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,350, 240,20);
	assert( itsFortranSuffixInput != nullptr );

	auto* dSourceLabel =
		jnew JXStaticText(JGetString("dSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,380, 170,20);
	assert( dSourceLabel != nullptr );
	dSourceLabel->SetToLabel();

	itsDSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,380, 240,20);
	assert( itsDSuffixInput != nullptr );

	auto* goSourceLabel =
		jnew JXStaticText(JGetString("goSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,410, 170,20);
	assert( goSourceLabel != nullptr );
	goSourceLabel->SetToLabel();

	itsGoSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,410, 240,20);
	assert( itsGoSuffixInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CMEditPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsGDBCmd->GetText()->SetText(gdbCmd);
	itsGDBCmd->SetIsRequired();
	itsGDBCmd->ShouldRequireExecutable();

	itsJDBCmd->GetText()->SetText(jdbCmd);
	itsJDBCmd->SetIsRequired();
	itsJDBCmd->ShouldRequireExecutable();

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsEditFileCmdInput->GetText()->SetText(editFileCmd);
	itsEditFileCmdInput->SetIsRequired();
	itsEditFileCmdInput->SetFont(font);

	itsEditFileLineCmdInput->GetText()->SetText(editFileLineCmd);
	itsEditFileLineCmdInput->SetIsRequired();
	itsEditFileLineCmdInput->SetFont(font);

	CBMSetStringList(itsCSourceSuffixInput, cSourceSuffixes);
	itsCSourceSuffixInput->SetIsRequired();
	itsCSourceSuffixInput->SetFont(font);

	CBMSetStringList(itsCHeaderSuffixInput, cHeaderSuffixes);
	itsCHeaderSuffixInput->SetIsRequired();
	itsCHeaderSuffixInput->SetFont(font);

	CBMSetStringList(itsJavaSuffixInput, javaSuffixes);
	itsJavaSuffixInput->SetIsRequired();
	itsJavaSuffixInput->SetFont(font);

	CBMSetStringList(itsPHPSuffixInput, phpSuffixes);
	itsPHPSuffixInput->SetIsRequired();
	itsPHPSuffixInput->SetFont(font);

	CBMSetStringList(itsFortranSuffixInput, fortranSuffixes);
	itsFortranSuffixInput->SetIsRequired();
	itsFortranSuffixInput->SetFont(font);

	CBMSetStringList(itsDSuffixInput, dSuffixes);
	itsDSuffixInput->SetIsRequired();
	itsDSuffixInput->SetFont(font);

	CBMSetStringList(itsGoSuffixInput, goSuffixes);
	itsGoSuffixInput->SetIsRequired();
	itsGoSuffixInput->SetFont(font);

	ListenTo(itsChooseGDBButton);
	ListenTo(itsChooseJDBButton);
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
CMEditPrefsDialog::GetPrefs
	(
	JString*			gdbCmd,
	JString*			jdbCmd,
	JString*			editFileCmd,
	JString*			editFileLineCmd,
	JPtrArray<JString>*	cSourceSuffixes,
	JPtrArray<JString>*	cHeaderSuffixes,
	JPtrArray<JString>*	javaSuffixes,
	JPtrArray<JString>*	phpSuffixes,
	JPtrArray<JString>*	fortranSuffixes,
	JPtrArray<JString>*	dSuffixes,
	JPtrArray<JString>*	goSuffixes
	)
	const
{
	*gdbCmd          = itsGDBCmd->GetText()->GetText();
	*jdbCmd          = itsJDBCmd->GetText()->GetText();
	*editFileCmd     = itsEditFileCmdInput->GetText()->GetText();
	*editFileLineCmd = itsEditFileLineCmdInput->GetText()->GetText();

	CBMGetSuffixList(itsCSourceSuffixInput, cSourceSuffixes);
	CBMGetSuffixList(itsCHeaderSuffixInput, cHeaderSuffixes);
	CBMGetSuffixList(itsJavaSuffixInput, javaSuffixes);
	CBMGetSuffixList(itsPHPSuffixInput, phpSuffixes);
	CBMGetSuffixList(itsFortranSuffixInput, fortranSuffixes);
	CBMGetSuffixList(itsDSuffixInput, dSuffixes);
	CBMGetSuffixList(itsGoSuffixInput, goSuffixes);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMEditPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseGDBButton && message.Is(JXButton::kPushed))
		{
		ChooseDebugger(JString("gdb", JString::kNoCopy), itsGDBCmd);
		}
	else if (sender == itsChooseJDBButton && message.Is(JXButton::kPushed))
		{
		ChooseDebugger(JString("jdb", JString::kNoCopy), itsJDBCmd);
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ChooseDebugger (private)

 ******************************************************************************/

void
CMEditPrefsDialog::ChooseDebugger
	(
	const JString&	name,
	JXInputField*	input
	)
{
	const JUtf8Byte* map[] =
		{
		"name", name.GetBytes()
		};
	const JString prompt = JGetString("ChooseDebuggerPrompt::CMEditPrefsDialog", map, sizeof(map));

	JString fullName;
	if (JGetChooseSaveFile()->ChooseFile(prompt, JString::empty, &fullName))
		{
		JString text = input->GetText()->GetText();

		JStringIterator iter(&text);
		iter.BeginMatch();
		if (iter.Next(" "))
			{
			iter.SkipPrev();
			iter.FinishMatch();
			iter.ReplaceLastMatch(fullName);
			}
		else
			{
			text = fullName;
			}
		iter.Invalidate();

		input->GetText()->SetText(text);
		}
}
