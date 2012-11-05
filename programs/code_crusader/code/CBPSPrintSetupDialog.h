/******************************************************************************
 CBPSPrintSetupDialog.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPSPrintSetupDialog
#define _H_CBPSPrintSetupDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPSPrintSetupDialog.h>

class JXFontSizeMenu;

class CBPSPrintSetupDialog : public JXPSPrintSetupDialog
{
public:

	static CBPSPrintSetupDialog*
		Create(const JXPSPrinter::Destination dest,
			   const JCharacter* printCmd, const JCharacter* fileName,
			   const JBoolean collate, const JBoolean bw,
			   const JSize fontSize,
			   const JBoolean printHeader);

	virtual ~CBPSPrintSetupDialog();

	void	CBGetSettings(JSize* fontSize, JBoolean* printHeader) const;

protected:

	CBPSPrintSetupDialog();

private:

// begin JXLayout

	JXTextCheckbox* itsPrintHeaderCB;
	JXFontSizeMenu* itsFontSizeMenu;

// end JXLayout

private:

	void	BuildWindow(const JXPSPrinter::Destination dest,
						const JCharacter* printCmd, const JCharacter* fileName,
						const JBoolean collate, const JBoolean bw,
						const JSize fontSize,
						const JBoolean printHeader);

	// not allowed

	CBPSPrintSetupDialog(const CBPSPrintSetupDialog& source);
	const CBPSPrintSetupDialog& operator=(const CBPSPrintSetupDialog& source);
};

#endif
