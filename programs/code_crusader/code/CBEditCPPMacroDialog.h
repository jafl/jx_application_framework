/******************************************************************************
 CBEditCPPMacroDialog.h

	Interface for the CBEditCPPMacroDialog class

	Copyright © 1998 by John Lindal.

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

	bool	UpdateMacros(CBCPreprocessor* cpp) const;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual bool	OKToDeactivate() override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
