/******************************************************************************
 CBEditProjPrefsDialog.h

	Interface for the CBEditProjPrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditProjPrefsDialog
#define _H_CBEditProjPrefsDialog

#include <JXDialogDirector.h>
#include "CBProjectTable.h"		// need definition of DropFileAction

class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;

class CBEditProjPrefsDialog : public JXDialogDirector
{
public:

	CBEditProjPrefsDialog(const bool reopenTextFiles,
						  const bool doubleSpaceCompile,
						  const bool rebuildMakefileDaily,
						  const CBProjectTable::DropFileAction dropFileAction);

	virtual ~CBEditProjPrefsDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsReopenTextFilesCB;
	JXTextButton*   itsHelpButton;
	JXRadioGroup*   itsDropFileActionRG;
	JXTextCheckbox* itsDoubleSpaceCB;
	JXTextCheckbox* itsRebuildMakefileDailyCB;

// end JXLayout

private:

	void	BuildWindow(const bool reopenTextFiles,
						const bool doubleSpaceCompile,
						const bool rebuildMakefileDaily,
						const CBProjectTable::DropFileAction dropFileAction);
	void	UpdateSettings();

	// not allowed

	CBEditProjPrefsDialog(const CBEditProjPrefsDialog& source);
	const CBEditProjPrefsDialog& operator=(const CBEditProjPrefsDialog& source);
};

#endif
