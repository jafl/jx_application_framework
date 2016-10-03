/******************************************************************************
 GNBPTPrintSetupDialog.h

	Copyright (C) 2000 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBPTPrintSetupDialog
#define _H_GNBPTPrintSetupDialog

#include <JXPTPrintSetupDialog.h>

class GNBPTPrintSetupDialog : public JXPTPrintSetupDialog
{
public:

	GNBPTPrintSetupDialog();

	virtual ~GNBPTPrintSetupDialog();

	void	BuildWindow(const JXPTPrinter::Destination dest,
						const JCharacter* printCmd, const JCharacter* fileName,
						const JBoolean printLineNumbers, const JBoolean printSel,
						const JBoolean printNotes, const JBoolean printClosed,
						const JBoolean hasSelection);

	void		ShouldPrintSelection(const JBoolean print);
	JBoolean	WillPrintSelection() const;

	void		ShouldPrintNotes(const JBoolean print);
	JBoolean	WillPrintNotes() const;

	void		ShouldPrintClosed(const JBoolean print);
	JBoolean	WillPrintClosed() const;

private:

// begin JXLayout

    JXTextCheckbox* itsPrintSelectionCB;
    JXTextCheckbox* itsPrintNotesCB;
    JXTextCheckbox* itsPrintClosedCB;

// end JXLayout

private:

	// not allowed

	GNBPTPrintSetupDialog(const GNBPTPrintSetupDialog& source);
	const GNBPTPrintSetupDialog& operator=(const GNBPTPrintSetupDialog& source);
};

#endif
