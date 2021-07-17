/******************************************************************************
 CBEditMacroDialog.h

	Interface for the CBEditMacroDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditMacroDialog
#define _H_CBEditMacroDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>
#include "CBPrefsManager.h"		// need definition of MacroSetInfo

class JXTextButton;
class JXVertPartition;
class CBMacroSetTable;
class CBCharActionTable;
class CBMacroTable;

class CBEditMacroDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBEditMacroDialog(JArray<CBPrefsManager::MacroSetInfo>* macroList,
					  const JIndex initialSelection,
					  const JIndex firstUnusedID);

	virtual ~CBEditMacroDialog();

	bool	ContentsValid() const;
	bool	GetCurrentMacroSetName(JString* name) const;

	JArray<CBPrefsManager::MacroSetInfo>*
		GetMacroList(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBMacroSetTable*	itsMacroSetTable;
	CBCharActionTable*	itsActionTable;
	CBMacroTable*		itsMacroTable;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin actionLayout


// end actionLayout

// begin macroLayout


// end macroLayout

// begin macroSetLayout


// end macroSetLayout

private:

	void	BuildWindow(JArray<CBPrefsManager::MacroSetInfo>* macroList,
						const JIndex initialSelection,
						const JIndex firstUnusedID);

	// not allowed

	CBEditMacroDialog(const CBEditMacroDialog& source);
	const CBEditMacroDialog& operator=(const CBEditMacroDialog& source);
};

#endif
