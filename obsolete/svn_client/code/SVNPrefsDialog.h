/******************************************************************************
 SVNPrefsDialog.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNPrefsDialog
#define _H_SVNPrefsDialog

#include <JXDialogDirector.h>
#include "SVNPrefsManager.h"	// need defn of Integration

class JXRadioGroup;
class JXTextRadioButton;
class JXInputField;

class SVNPrefsDialog : public JXDialogDirector
{
public:

	SVNPrefsDialog(JXDirector* supervisor,
				   const SVNPrefsManager::Integration type,
				   const JCharacter* commitEditor,
				   const JCharacter* diffCmd,
				   const JCharacter* reloadChangedCmd);

	virtual	~SVNPrefsDialog();

	void	GetData(SVNPrefsManager::Integration* type, JString* commitEditor,
					JString* diffCmd, JString* reloadChangedCmd) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXInputField*      itsCommitEditor;
	JXInputField*      itsDiffCmd;
	JXRadioGroup*      itsIntegrationRG;
	JXTextRadioButton* itsJCCIntegrationRB;
	JXTextRadioButton* itsCustomIntegrationRB;
	JXTextRadioButton* itsCmdLineIntegrationRB;
	JXInputField*      itsReloadChangedCmd;

// end JXLayout

private:

	void	BuildWindow(const SVNPrefsManager::Integration type,
						const JCharacter* commitEditor, const JCharacter* diffCmd,
						const JCharacter* reloadChangedCmd);
	void	UpdateDisplay();

	// not allowed

	SVNPrefsDialog(const SVNPrefsDialog& source);
	const SVNPrefsDialog& operator=(const SVNPrefsDialog& source);
};

#endif
