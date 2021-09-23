/******************************************************************************
 JXEPSPrintSetupDialog.h

	Interface for the JXEPSPrintSetupDialog class

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEPSPrintSetupDialog
#define _H_JXEPSPrintSetupDialog

#include "JXDialogDirector.h"

class JXEPSPrinter;
class JXTextButton;
class JXStaticText;
class JXTextCheckbox;
class JXFileInput;

class JXEPSPrintSetupDialog : public JXDialogDirector
{
public:

	static JXEPSPrintSetupDialog*
		Create(const JString& fileName,
			   const bool printPreview, const bool bw);

	virtual ~JXEPSPrintSetupDialog();

	bool	SetParameters(JXEPSPrinter* p) const;

	void	ChooseDestinationFile();

protected:

	JXEPSPrintSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXFileInput* fileInput, const JString& fileName,
					   JXTextButton* chooseFileButton,
					   JXTextCheckbox* previewCheckbox, const bool printPreview,
					   JXTextCheckbox* bwCheckbox, const bool bw);

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

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
