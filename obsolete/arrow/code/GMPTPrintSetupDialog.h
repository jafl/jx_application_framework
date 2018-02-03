/******************************************************************************
 GMPTPrintSetupDialog.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GMPTPrintSetupDialog
#define _H_GMPTPrintSetupDialog

#include <JXPTPrintSetupDialog.h>

class GMPTPrintSetupDialog : public JXPTPrintSetupDialog
{
public:

	GMPTPrintSetupDialog();

	virtual ~GMPTPrintSetupDialog();

	void	BuildWindow(const JXPTPrinter::Destination dest,
						const JCharacter* printCmd, const JCharacter* fileName,
						const JBoolean printLineNumbers, const JBoolean printHeader);

	void		ShouldPrintHeader(const JBoolean print);
	JBoolean	WillPrintHeader() const;

private:

// begin JXLayout

	JXTextCheckbox* itsPrintHeadersCB;

// end JXLayout

private:

	// not allowed

	GMPTPrintSetupDialog(const GMPTPrintSetupDialog& source);
	const GMPTPrintSetupDialog& operator=(const GMPTPrintSetupDialog& source);
};

#endif
