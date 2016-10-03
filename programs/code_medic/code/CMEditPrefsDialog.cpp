/******************************************************************************
 CMEditPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2001-10 by John Lindal. All rights reserved.

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

	JXWindow* window = new JXWindow(this, 450,420, "");
	assert( window != NULL );

	itsGDBCmd =
		new CMDebuggerProgramInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 350,20);
	assert( itsGDBCmd != NULL );

	itsJDBCmd =
		new CMDebuggerProgramInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,110, 350,20);
	assert( itsJDBCmd != NULL );

	itsEditFileCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,160, 240,20);
	assert( itsEditFileCmdInput != NULL );

	itsEditFileLineCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,180, 240,20);
	assert( itsEditFileLineCmdInput != NULL );

	itsCSourceSuffixInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,240, 240,20);
	assert( itsCSourceSuffixInput != NULL );

	itsCHeaderSuffixInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,260, 240,20);
	assert( itsCHeaderSuffixInput != NULL );

	itsJavaSuffixInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,290, 240,20);
	assert( itsJavaSuffixInput != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,390, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CMEditPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,390, 60,20);
	assert( cancelButton != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,240, 170,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,260, 170,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,200, 270,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetFontSize(8);
	obj3_JXLayout->SetToLabel();

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 170,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 170,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	JXStaticText* obj6_JXLayout =
		new JXStaticText(JGetString("obj6_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,290, 159,20);
	assert( obj6_JXLayout != NULL );
	obj6_JXLayout->SetToLabel();

	JXStaticText* obj7_JXLayout =
		new JXStaticText(JGetString("obj7_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,350, 170,20);
	assert( obj7_JXLayout != NULL );
	obj7_JXLayout->SetToLabel();

	JXStaticText* obj8_JXLayout =
		new JXStaticText(JGetString("obj8_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 340,20);
	assert( obj8_JXLayout != NULL );
	obj8_JXLayout->SetToLabel();

	itsChooseGDBButton =
		new JXTextButton(JGetString("itsChooseGDBButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,40, 60,20);
	assert( itsChooseGDBButton != NULL );

	JXStaticText* obj9_JXLayout =
		new JXStaticText(JGetString("obj9_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 410,20);
	assert( obj9_JXLayout != NULL );
	obj9_JXLayout->SetFontSize(8);
	obj9_JXLayout->SetToLabel();

	JXStaticText* obj10_JXLayout =
		new JXStaticText(JGetString("obj10_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 340,20);
	assert( obj10_JXLayout != NULL );
	obj10_JXLayout->SetToLabel();

	itsChooseJDBButton =
		new JXTextButton(JGetString("itsChooseJDBButton::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,110, 60,20);
	assert( itsChooseJDBButton != NULL );

	JXStaticText* obj11_JXLayout =
		new JXStaticText(JGetString("obj11_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 410,20);
	assert( obj11_JXLayout != NULL );
	obj11_JXLayout->SetFontSize(8);
	obj11_JXLayout->SetToLabel();

	itsPHPSuffixInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,320, 240,20);
	assert( itsPHPSuffixInput != NULL );

	JXStaticText* obj12_JXLayout =
		new JXStaticText(JGetString("obj12_JXLayout::CMEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,320, 159,20);
	assert( obj12_JXLayout != NULL );
	obj12_JXLayout->SetToLabel();

	itsFortranSuffixInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,350, 240,20);
	assert( itsFortranSuffixInput != NULL );

// end JXLayout

	window->SetTitle("Edit preferences");
	SetButtons(okButton, cancelButton);

	itsGDBCmd->SetText(gdbCmd);
	itsGDBCmd->SetIsRequired();
	itsGDBCmd->ShouldRequireExecutable();

	itsJDBCmd->SetText(jdbCmd);
	itsJDBCmd->SetIsRequired();
	itsJDBCmd->ShouldRequireExecutable();

	const JFont& font = window->GetFontManager()->GetDefaultMonospaceFont();

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
	if ((JGetChooseSaveFile())->ChooseFile(prompt, "", &fullName))
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
