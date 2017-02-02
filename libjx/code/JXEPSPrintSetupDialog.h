/******************************************************************************
 JXEPSPrintSetupDialog.h

	Interface for the JXEPSPrintSetupDialog class

	Copyright (C) 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXEPSPrintSetupDialog
#define _H_JXEPSPrintSetupDialog

#include <JXDialogDirector.h>

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
			   const JBoolean printPreview, const JBoolean bw);

	virtual ~JXEPSPrintSetupDialog();

	JBoolean	SetParameters(JXEPSPrinter* p) const;

protected:

	JXEPSPrintSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXFileInput* fileInput, const JString& fileName,
					   JXTextButton* chooseFileButton,
					   JXTextCheckbox* previewCheckbox, const JBoolean printPreview,
					   JXTextCheckbox* bwCheckbox, const JBoolean bw);

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

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
						const JBoolean printPreview, const JBoolean bw);
	void	UpdateDisplay();

	void	ChooseDestinationFile();

	// not allowed

	JXEPSPrintSetupDialog(const JXEPSPrintSetupDialog& source);
	const JXEPSPrintSetupDialog& operator=(const JXEPSPrintSetupDialog& source);
};

#endif
