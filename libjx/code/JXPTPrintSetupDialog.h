/******************************************************************************
 JXPTPrintSetupDialog.h

	Interface for the JXPTPrintSetupDialog class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPTPrintSetupDialog
#define _H_JXPTPrintSetupDialog

#include "jx-af/jx/JXDialogDirector.h"
#include "jx-af/jx/JXPTPrinter.h"		// need definition of enums

class JXTextButton;
class JXStaticText;
class JXInputField;
class JXIntegerInput;
class JXFileInput;
class JXTextCheckbox;
class JXRadioGroup;

class JXPTPrintSetupDialog : public JXDialogDirector
{
public:

	static JXPTPrintSetupDialog*
		Create(const JXPTPrinter::Destination dest,
			   const JString& printCmd, const JString& fileName,
			   const bool printLineNumbers);

	virtual ~JXPTPrintSetupDialog();

	bool	SetParameters(JXPTPrinter* p) const;

protected:

	JXPTPrintSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXRadioGroup* destinationRG, const JXPTPrinter::Destination dest,
					   JXStaticText* printCmdLabel, JXInputField* printCmdInput,
					   const JString& printCmd, JXTextButton* chooseFileButton,
					   const JString& fileName, JXIntegerInput* copyCount,
					   JXTextCheckbox* printAllCheckbox,
					   JXStaticText* firstPageIndexLabel, JXIntegerInput* firstPageIndex,
					   JXStaticText* lastPageIndexLabel, JXIntegerInput* lastPageIndex,
					   JXTextCheckbox* printLineNumbersCB, const bool printLineNumbers);

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFileInput*	itsFileInput;
	JXTextButton*	itsPrintButton;

// begin JXLayout

	JXStaticText*   itsPrintCmdLabel;
	JXRadioGroup*   itsDestination;
	JXInputField*   itsPrintCmd;
	JXTextButton*   itsChooseFileButton;
	JXIntegerInput* itsCopyCount;
	JXIntegerInput* itsFirstPageIndex;
	JXIntegerInput* itsLastPageIndex;
	JXTextCheckbox* itsPrintAllCB;
	JXStaticText*   itsFirstPageIndexLabel;
	JXStaticText*   itsLastPageIndexLabel;
	JXTextCheckbox* itsPrintLineNumbersCB;

// end JXLayout

private:

	void	BuildWindow(const JXPTPrinter::Destination dest,
						const JString& printCmd, const JString& fileName,
						const bool printLineNumbers);
	void	UpdateDisplay();

	void	SetDestination(const JIndex id);
	void	PrintAllPages(const bool all);

	void	ChooseDestinationFile();
};

#endif
