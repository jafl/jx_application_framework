/******************************************************************************
 JXPSPageSetupDialog.h

	Interface for the JXPSPageSetupDialog class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPSPageSetupDialog
#define _H_JXPSPageSetupDialog

#include "JXModalDialogDirector.h"
#include <jx-af/jcore/JPSPrinter.h>		// need definition of enums

class JXPSPrinter;
class JXTextButton;
class JXTextMenu;
class JXRadioGroup;
class JXImageRadioButton;

class JXPSPageSetupDialog : public JXModalDialogDirector
{
public:

	static JXPSPageSetupDialog*
		Create(const JPSPrinter::PaperType paper,
			   const JPSPrinter::ImageOrientation orient);

	~JXPSPageSetupDialog() override;

	virtual bool	SetParameters(JXPSPrinter* p) const;

protected:

	JXPSPageSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXTextMenu* paperTypeMenu, const JPSPrinter::PaperType paper,
					   JXRadioGroup* orientationRG, JXImageRadioButton* portraitRB,
					   JXImageRadioButton* landscapeRB,
					   const JPSPrinter::ImageOrientation orient);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsPaperType;

// begin JXLayout

	JXRadioGroup* itsOrientation;
	JXTextMenu*   itsPaperTypeMenu;

// end JXLayout

private:

	void	BuildWindow(const JPSPrinter::PaperType paper,
						const JPSPrinter::ImageOrientation orient);

	void	UpdatePaperTypeMenu();
	void	HandlePaperTypeMenu(const JIndex item);
};

#endif
