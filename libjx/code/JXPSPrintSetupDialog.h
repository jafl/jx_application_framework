/******************************************************************************
 JXPSPrintSetupDialog.h

	Interface for the JXPSPrintSetupDialog class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPSPrintSetupDialog
#define _H_JXPSPrintSetupDialog

#include "jx-af/jx/JXDialogDirector.h"
#include "jx-af/jx/JXPSPrinter.h"		// need definition of enums

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
			   const JString& printCmd, const JString& fileName,
			   const bool collate, const bool bw);

	~JXPSPrintSetupDialog() override;

	bool	SetParameters(JXPSPrinter* p) const;

	static bool	OKToDeactivate(const JString& fullName);

protected:

	JXPSPrintSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXRadioGroup* destinationRG, const JXPSPrinter::Destination dest,
					   JXStaticText* printCmdLabel, JXInputField* printCmdInput,
					   const JString& printCmd, JXTextButton* chooseFileButton,
					   const JString& fileName, JXIntegerInput* copyCount,
					   JXTextCheckbox* collateCheckbox, const bool collate,
					   JXTextCheckbox* bwCheckbox, const bool bw,
					   JXTextCheckbox* printAllCheckbox,
					   JXStaticText* firstPageIndexLabel, JXIntegerInput* firstPageIndex,
					   JXStaticText* lastPageIndexLabel, JXIntegerInput* lastPageIndex);

	bool	OKToDeactivate() override;
	void		Receive(JBroadcaster* sender, const Message& message) override;

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
						const JString& printCmd, const JString& fileName,
						const bool collate, const bool bw);
	void	UpdateDisplay();

	void	SetDestination(const JIndex id);
	void	PrintAllPages(const bool all);

	void	ChooseDestinationFile();
};

#endif
