/******************************************************************************
 CBProjectConfigDialog.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBProjectConfigDialog
#define _H_CBProjectConfigDialog

#include <JXDialogDirector.h>
#include "CBBuildManager.h"		// need defn on MakefileMethod

class JString;
class JXTextButton;
class JXRadioGroup;
class JXInputField;

class CBProjectConfigDialog : public JXDialogDirector
{
public:

	CBProjectConfigDialog(JXDirector* supervisor,
						   const CBBuildManager::MakefileMethod method,
						   const JCharacter* targetName,
						   const JCharacter* depListExpr,
						   const JCharacter* updateMakefileCmd,
						   const JCharacter* subProjectBuildCmd);

	virtual ~CBProjectConfigDialog();

	void	GetConfig(CBBuildManager::MakefileMethod* method,
					  JString* targetName, JString* depListExpr,
					  JString* updateMakefileCmd, JString* subProjectBuildCmd) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBBuildManager::MakefileMethod	itsCurrentMethod;

// begin JXLayout

	JXRadioGroup* itsMethodRG;
	JXInputField* itsTargetName;
	JXInputField* itsDepListExpr;
	JXInputField* itsUpdateMakefileCmd;
	JXInputField* itsSubProjectBuildCmd;
	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const CBBuildManager::MakefileMethod method,
						const JCharacter* targetName,
						const JCharacter* depListExpr,
						const JCharacter* updateMakefileCmd,
						const JCharacter* subProjectBuildCmd);

	void	UpdateDisplay();

	// not allowed

	CBProjectConfigDialog(const CBProjectConfigDialog& source);
	const CBProjectConfigDialog& operator=(const CBProjectConfigDialog& source);
};

#endif
