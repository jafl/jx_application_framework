/******************************************************************************
 CBEditCRMDialog.h

	Interface for the CBEditCRMDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditCRMDialog
#define _H_CBEditCRMDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>
#include "CBPrefsManager.h"		// need definition of CRMRuleListInfo

class JXTextButton;
class JXVertPartition;
class CBCRMRuleListTable;
class CBCRMRuleTable;

class CBEditCRMDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBEditCRMDialog(JArray<CBPrefsManager::CRMRuleListInfo>* crmList,
					const JIndex initialSelection,
					const JIndex firstUnusedID);

	virtual ~CBEditCRMDialog();

	bool	GetCurrentCRMRuleSetName(JString* name) const;

	JArray<CBPrefsManager::CRMRuleListInfo>*
		GetCRMRuleLists(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBCRMRuleListTable*	itsCRMTable;
	CBCRMRuleTable*		itsRuleTable;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin ruleLayout


// end ruleLayout

// begin crmLayout


// end crmLayout

private:

	void	BuildWindow(JArray<CBPrefsManager::CRMRuleListInfo>* crmList,
						const JIndex initialSelection,
						const JIndex firstUnusedID);

	// not allowed

	CBEditCRMDialog(const CBEditCRMDialog& source);
	const CBEditCRMDialog& operator=(const CBEditCRMDialog& source);
};

#endif
