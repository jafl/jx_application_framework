/******************************************************************************
 CBPTPrintSetupDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPTPrintSetupDialog
#define _H_CBPTPrintSetupDialog

#include <JXPTPrintSetupDialog.h>

class CBPTPrintSetupDialog : public JXPTPrintSetupDialog
{
public:

	static CBPTPrintSetupDialog*
		Create(const JXPTPrinter::Destination dest,
			   const JString& printCmd, const JString& fileName,
			   const bool printLineNumbers, const bool printHeader);

	virtual ~CBPTPrintSetupDialog();

	bool	ShouldPrintHeader() const;

protected:

	CBPTPrintSetupDialog();

private:

// begin JXLayout

	JXTextCheckbox* itsPrintHeaderCB;

// end JXLayout

private:

	void	BuildWindow(const JXPTPrinter::Destination dest,
						const JString& printCmd, const JString& fileName,
						const bool printLineNumbers, const bool printHeader);

	// not allowed

	CBPTPrintSetupDialog(const CBPTPrintSetupDialog& source);
	const CBPTPrintSetupDialog& operator=(const CBPTPrintSetupDialog& source);
};

#endif
