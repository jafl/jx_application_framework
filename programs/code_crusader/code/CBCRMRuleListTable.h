/******************************************************************************
 CBCRMRuleListTable.h

	Interface for the CBCRMRuleListTable class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCRMRuleListTable
#define _H_CBCRMRuleListTable

#include <JXStringTable.h>
#include "CBPrefsManager.h"		// need definition of CRMRuleListInfo

class JXTextButton;
class CBCRMRuleTable;

class CBCRMRuleListTable : public JXStringTable
{
public:

	CBCRMRuleListTable(JArray<CBPrefsManager::CRMRuleListInfo>* crmList,
					   const JIndex initialSelection, const JIndex firstUnusedID,
					   CBCRMRuleTable* ruleTable,
					   JXTextButton* addRowButton, JXTextButton* removeRowButton,
					   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~CBCRMRuleListTable();

	bool	GetCurrentCRMRuleSetName(JString* name) const;

	JArray<CBPrefsManager::CRMRuleListInfo>*
		GetCRMRuleLists(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JArray<CBPrefsManager::CRMRuleListInfo>*	itsCRMList;

	mutable bool	itsOwnsCRMListFlag;		// true => delete contents of itsMacroList
	const JIndex	itsFirstNewID;			// first index to use for new sets
	JIndex			itsLastNewID;			// index of last new set created
	JIndex			itsCRMIndex;			// index of currently displayed macro set

	CBCRMRuleTable*	itsRuleTable;

	JXTextButton*	itsAddRowButton;
	JXTextButton*	itsRemoveRowButton;

private:

	void	AddRow();
	void	RemoveRow();
	void	SwitchDisplay();

	static JListT::CompareResult
		CompareNames(
			const CBPrefsManager::CRMRuleListInfo& i1,
			const CBPrefsManager::CRMRuleListInfo& i2);

	// not allowed

	CBCRMRuleListTable(const CBCRMRuleListTable& source);
	const CBCRMRuleListTable& operator=(const CBCRMRuleListTable& source);
};

#endif
