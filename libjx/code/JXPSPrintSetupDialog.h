/******************************************************************************
 JXPSPrintSetupDialog.h

	Interface for the JXPSPrintSetupDialog class

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPSPrintSetupDialog
#define _H_JXPSPrintSetupDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JXPSPrinter.h>		// need definition of enums

class JXTextButton;
class JXStaticText;
class JXInputField;
class JXIntegerInput;
class JXFileInput;
class JXTextCheckbox;
class JXRadioGroup;

class JXPSPrintSetupDialog : public JXDialogDirector
{
public:

	static JXPSPrintSetupDialog*
		Create(const JXPSPrinter::Destination dest,
			   const JCharacter* printCmd, const JCharacter* fileName,
			   const JBoolean collate, const JBoolean bw);

	virtual ~JXPSPrintSetupDialog();

	JBoolean	SetParameters(JXPSPrinter* p) const;

	static JBoolean	OKToDeactivate(const JString& fullName);

protected:

	JXPSPrintSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXRadioGroup* destinationRG, const JXPSPrinter::Destination dest,
					   JXStaticText* printCmdLabel, JXInputField* printCmdInput,
					   const JCharacter* printCmd, JXTextButton* chooseFileButton,
					   const JCharacter* fileName, JXIntegerInput* copyCount,
					   JXTextCheckbox* collateCheckbox, const JBoolean collate,
					   JXTextCheckbox* bwCheckbox, const JBoolean bw,
					   JXTextCheckbox* printAllCheckbox,
					   JXStaticText* firstPageIndexLabel, JXIntegerInput* firstPageIndex,
					   JXStaticText* lastPageIndexLabel, JXIntegerInput* lastPageIndex);

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

    JXFileInput*	itsFileInput;
    JXTextButton*	itsPrintButton;

// begin JXLayout

    JXStaticText*   itsPrintCmdLabel;
    JXRadioGroup*   itsDestination;
    JXInputField*   itsPrintCmd;
    JXTextButton*   itsChooseFileButton;
    JXIntegerInput* itsCopyCount;
    JXTextCheckbox* itsBWCheckbox;
    JXIntegerInput* itsFirstPageIndex;
    JXIntegerInput* itsLastPageIndex;
    JXTextCheckbox* itsPrintAllCB;
    JXStaticText*   itsFirstPageIndexLabel;
    JXStaticText*   itsLastPageIndexLabel;
    JXTextCheckbox* itsCollateCB;

// end JXLayout

private:

	void	BuildWindow(const JXPSPrinter::Destination dest,
						const JCharacter* printCmd, const JCharacter* fileName,
						const JBoolean collate, const JBoolean bw);

	void	SetDestination(const JIndex id);
	void	PrintAllPages(const JBoolean all);

	void	ChooseDestinationFile();

	// not allowed

	JXPSPrintSetupDialog(const JXPSPrintSetupDialog& source);
	const JXPSPrintSetupDialog& operator=(const JXPSPrintSetupDialog& source);
};

#endif
