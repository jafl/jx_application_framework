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
#include <JXFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMEditPrefsDialog::CMEditPrefsDialog
	(
	JXDirector*					supervisor,
	const JCharacter*			gdbCmd,
	const JCharacter*			jdbCmd,
	const JCharacter*			editFileCmd,
	const JCharacter*			editFileLineCmd,
	const JPtrArray<JString>&	cSourceSuffixes,
	const JPtrArray<JString>&	cHeaderSuffixes,
	const JPtrArray<JString>&	javaSuffixes,
	const JPtrArray<JString>&	phpSuffixes,
	const JPtrArray<JString>&	fortranSuffixes
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(gdbCmd, jdbCmd, editFileCmd, editFileLineCmd,
				cSourceSuffixes, cHeaderSuffixes,
				javaSuffixes, phpSuffixes, fortranSuffixes);
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
	const JCharacter*			gdbCmd,
	const JCharacter*			jdbCmd,
	const JCharacter*			editFileCmd,
	const JCharacter*			editFileLineCmd,
	const JPtrArray<JString>&	cSourceSuffixes,
	const JPtrArray<JString>&	cHeaderSuffixes,
	const JPtrArray<JString>&	javaSuffixes,
	const JPtrArray<JString>&	phpSuffixes,
	const JPtrArray<JString>&	fortranSuffixes
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 450,420, "");
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

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,390, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CMEditPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,390, 60,20);
	assert( cancelButton != nullptr );

	JXStaticText* cSourceFileLabel =
		jnew JXStaticText(JGetString("cSourceFileLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,240, 170,20);
	assert( cSourceFileLabel != nullptr );
	cSourceFileLabel->SetToLabel();

	JXStaticText* cHeaderFileLabel =
		jnew JXStaticText(JGetString("cHeaderFileLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,260, 170,20);
	assert( cHeaderFileLabel != nullptr );
	cHeaderFileLabel->SetToLabel();

	JXStaticText* cmdHint =
		jnew JXStaticText(JGetString("cmdHint::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,200, 270,20);
	assert( cmdHint != nullptr );
	cmdHint->SetFontSize(JGetDefaultFontSize()-2);
	cmdHint->SetToLabel();

	JXStaticText* editFileLabel =
		jnew JXStaticText(JGetString("editFileLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 170,20);
	assert( editFileLabel != nullptr );
	editFileLabel->SetToLabel();

	JXStaticText* editLineLabel =
		jnew JXStaticText(JGetString("editLineLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 170,20);
	assert( editLineLabel != nullptr );
	editLineLabel->SetToLabel();

	JXStaticText* javaSourceLabel =
		jnew JXStaticText(JGetString("javaSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,290, 159,20);
	assert( javaSourceLabel != nullptr );
	javaSourceLabel->SetToLabel();

	JXStaticText* fortranSourceLabel =
		jnew JXStaticText(JGetString("fortranSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,350, 170,20);
	assert( fortranSourceLabel != nullptr );
	fortranSourceLabel->SetToLabel();

	JXStaticText* gdbCmdLabel =
		jnew JXStaticText(JGetString("gdbCmdLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 340,20);
	assert( gdbCmdLabel != nullptr );
	gdbCmdLabel->SetToLabel();

	itsChooseGDBButton =
		jnew JXTextButton(JGetString("itsChooseGDBButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,40, 60,20);
	assert( itsChooseGDBButton != nullptr );

	JXStaticText* gdbScriptHint =
		jnew JXStaticText(JGetString("gdbScriptHint::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 410,20);
	assert( gdbScriptHint != nullptr );
	gdbScriptHint->SetFontSize(JGetDefaultFontSize()-2);
	gdbScriptHint->SetToLabel();

	JXStaticText* jvmCmdLabel =
		jnew JXStaticText(JGetString("jvmCmdLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 340,20);
	assert( jvmCmdLabel != nullptr );
	jvmCmdLabel->SetToLabel();

	itsChooseJDBButton =
		jnew JXTextButton(JGetString("itsChooseJDBButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,110, 60,20);
	assert( itsChooseJDBButton != nullptr );

	JXStaticText* jvmScriptHint =
		jnew JXStaticText(JGetString("jvmScriptHint::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 410,20);
	assert( jvmScriptHint != nullptr );
	jvmScriptHint->SetFontSize(JGetDefaultFontSize()-2);
	jvmScriptHint->SetToLabel();

	itsPHPSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,320, 240,20);
	assert( itsPHPSuffixInput != nullptr );

	JXStaticText* phpSourceLabel =
		jnew JXStaticText(JGetString("phpSourceLabel::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,320, 159,20);
	assert( phpSourceLabel != nullptr );
	phpSourceLabel->SetToLabel();

	itsFortranSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,350, 240,20);
	assert( itsFortranSuffixInput != nullptr );

// end JXLayout

	window->SetTitle("Edit preferences");
	SetButtons(okButton, cancelButton);

	itsGDBCmd->SetText(gdbCmd);
	itsGDBCmd->SetIsRequired();
	itsGDBCmd->ShouldRequireExecutable();

	itsJDBCmd->SetText(jdbCmd);
	itsJDBCmd->SetIsRequired();
	itsJDBCmd->ShouldRequireExecutable();

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsEditFileCmdInput->SetText(editFileCmd);
	itsEditFileCmdInput->SetIsRequired();
	itsEditFileCmdInput->SetFont(font);

	itsEditFileLineCmdInput->SetText(editFileLineCmd);
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
	JPtrArray<JString>*	fortranSuffixes
	)
	const
{
	*gdbCmd          = itsGDBCmd->GetText();
	*jdbCmd          = itsJDBCmd->GetText();
	*editFileCmd     = itsEditFileCmdInput->GetText();
	*editFileLineCmd = itsEditFileLineCmdInput->GetText();

	CBMGetSuffixList(itsCSourceSuffixInput, cSourceSuffixes);
	CBMGetSuffixList(itsCHeaderSuffixInput, cHeaderSuffixes);
	CBMGetSuffixList(itsJavaSuffixInput, javaSuffixes);
	CBMGetSuffixList(itsPHPSuffixInput, phpSuffixes);
	CBMGetSuffixList(itsFortranSuffixInput, fortranSuffixes);
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
		ChooseDebugger("gdb", itsGDBCmd);
		}
	else if (sender == itsChooseJDBButton && message.Is(JXButton::kPushed))
		{
		ChooseDebugger("jdb", itsJDBCmd);
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
	const JCharacter*	name,
	JXInputField*		input
	)
{
	const JCharacter* map[] =
		{
		"name", name
		};
	const JString prompt = JGetString("ChooseDebuggerPrompt::CMEditPrefsDialog", map, sizeof(map));

	JString fullName;
	if (JGetChooseSaveFile()->ChooseFile(prompt, "", &fullName))
		{
		JString text = input->GetText();
		JIndex i;
		if (text.LocateSubstring(" ", &i))
			{
			text.ReplaceSubstring(1, i-1, fullName);
			}
		else
			{
			text = fullName;
			}
		input->SetText(text);
		}
}
