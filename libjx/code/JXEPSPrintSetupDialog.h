/******************************************************************************
 JXEPSPrintSetupDialog.h

	Interface for the JXEPSPrintSetupDialog class

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEPSPrintSetupDialog
#define _H_JXEPSPrintSetupDialog

#include "JXModalDialogDirector.h"

class JXEPSPrinter;
class JXTextButton;
class JXStaticText;
class JXTextCheckbox;
class JXFileInput;

class JXEPSPrintSetupDialog : public JXModalDialogDirector
{
public:

	static JXEPSPrintSetupDialog*
		Create(const JString& fileName,
			   const bool printPreview, const bool bw);

	~JXEPSPrintSetupDialog() override;

	virtual bool	SetParameters(JXEPSPrinter* p) const;

	void	ChooseDestinationFile();

protected:

	JXEPSPrintSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXFileInput* fileInput, const JString& fileName,
					   JXTextButton* chooseFileButton,
					   JXTextCheckbox* previewCheckbox, const bool printPreview,
					   JXTextCheckbox* bwCheckbox, const bool bw);

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextButton*	itsPrintButton;

// begin JXLayout

	JXTextButton*   itsChooseFileButton;
	JXTextCheckbox* itsBWCheckbox;
	JXTextCheckbox* itsPreviewCheckbox;
	JXFileInput*    itsFileInput;

// end JXLayout

private:

	void	BuildWindow(const JString& fileName,
						const bool printPreview, const bool bw);
	void	UpdateDisplay();
};

#endif
