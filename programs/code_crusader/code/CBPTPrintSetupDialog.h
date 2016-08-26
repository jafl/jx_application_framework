/******************************************************************************
 CBPTPrintSetupDialog.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPTPrintSetupDialog
#define _H_CBPTPrintSetupDialog

#include <JXPTPrintSetupDialog.h>

class CBPTPrintSetupDialog : public JXPTPrintSetupDialog
{
public:

	static CBPTPrintSetupDialog*
		Create(const JXPTPrinter::Destination dest,
			   const JCharacter* printCmd, const JCharacter* fileName,
			   const JBoolean printLineNumbers, const JBoolean printHeader);

	virtual ~CBPTPrintSetupDialog();

	JBoolean	ShouldPrintHeader() const;

protected:

	CBPTPrintSetupDialog();

private:

// begin JXLayout

	JXTextCheckbox* itsPrintHeaderCB;

// end JXLayout

private:

	void	BuildWindow(const JXPTPrinter::Destination dest,
						const JCharacter* printCmd, const JCharacter* fileName,
						const JBoolean printLineNumbers, const JBoolean printHeader);

	// not allowed

	CBPTPrintSetupDialog(const CBPTPrintSetupDialog& source);
	const CBPTPrintSetupDialog& operator=(const CBPTPrintSetupDialog& source);
};

#endif
