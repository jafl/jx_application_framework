/******************************************************************************
 CBPSPrintSetupDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPSPrintSetupDialog
#define _H_CBPSPrintSetupDialog

#include <JXPSPrintSetupDialog.h>

class JXFontSizeMenu;

class CBPSPrintSetupDialog : public JXPSPrintSetupDialog
{
public:

	static CBPSPrintSetupDialog*
		Create(const JXPSPrinter::Destination dest,
			   const JString& printCmd, const JString& fileName,
			   const bool collate, const bool bw,
			   const JSize fontSize,
			   const bool printHeader);

	virtual ~CBPSPrintSetupDialog();

	void	CBGetSettings(JSize* fontSize, bool* printHeader) const;

protected:

	CBPSPrintSetupDialog();

private:

// begin JXLayout

	JXTextCheckbox* itsPrintHeaderCB;
	JXFontSizeMenu* itsFontSizeMenu;

// end JXLayout

private:

	void	BuildWindow(const JXPSPrinter::Destination dest,
						const JString& printCmd, const JString& fileName,
						const bool collate, const bool bw,
						const JSize fontSize,
						const bool printHeader);

	// not allowed

	CBPSPrintSetupDialog(const CBPSPrintSetupDialog& source);
	const CBPSPrintSetupDialog& operator=(const CBPSPrintSetupDialog& source);
};

#endif
