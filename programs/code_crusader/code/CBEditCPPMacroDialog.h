/******************************************************************************
 CBEditCPPMacroDialog.h

	Interface for the CBEditCPPMacroDialog class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEditCPPMacroDialog
#define _H_CBEditCPPMacroDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JXTextButton;
class CBCPPMacroTable;
class CBCPreprocessor;

class CBEditCPPMacroDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBEditCPPMacroDialog(JXDirector* supervisor, const CBCPreprocessor& cpp);

	virtual ~CBEditCPPMacroDialog();

	JBoolean	UpdateMacros(CBCPreprocessor* cpp) const;

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual JBoolean	OKToDeactivate();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBCPPMacroTable*	itsTable;

// begin JXLayout

	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const CBCPreprocessor& cpp);

	// not allowed

	CBEditCPPMacroDialog(const CBEditCPPMacroDialog& source);
	const CBEditCPPMacroDialog& operator=(const CBEditCPPMacroDialog& source);
};

#endif
