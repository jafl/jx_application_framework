/******************************************************************************
 CBEditFileTypesDialog.h

	Interface for the CBEditFileTypesDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditFileTypesDialog
#define _H_CBEditFileTypesDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>
#include "CBPrefsManager.h"		// need definition of FileTypeInfo

class JXTextButton;
class JXTextCheckbox;
class CBFileTypeTable;

class CBEditFileTypesDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBEditFileTypesDialog(JXDirector* supervisor,
						  const JArray<CBPrefsManager::FileTypeInfo>& fileTypeList,
						  const JArray<CBPrefsManager::MacroSetInfo>& macroList,
						  const JArray<CBPrefsManager::CRMRuleListInfo>& crmList,
						  const bool execOutputWordWrap,
						  const bool unknownTypeWordWrap);

	virtual ~CBEditFileTypesDialog();

	void	GetFileTypeInfo(JArray<CBPrefsManager::FileTypeInfo>* fileTypeList,
							bool* execOutputWordWrap,
							bool* unknownTypeWordWrap) const;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBFileTypeTable*	itsTable;

// begin JXLayout

	JXTextCheckbox* itsExecOutputWrapCB;
	JXTextCheckbox* itsUnknownTypeWrapCB;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JArray<CBPrefsManager::FileTypeInfo>& fileTypeList,
						const JArray<CBPrefsManager::MacroSetInfo>& macroList,
						const JArray<CBPrefsManager::CRMRuleListInfo>& crmList,
						const bool execOutputWordWrap,
						const bool unknownTypeWordWrap);

	// not allowed

	CBEditFileTypesDialog(const CBEditFileTypesDialog& source);
	const CBEditFileTypesDialog& operator=(const CBEditFileTypesDialog& source);
};

#endif
