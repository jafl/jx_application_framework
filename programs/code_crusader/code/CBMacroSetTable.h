/******************************************************************************
 CBMacroSetTable.h

	Interface for the CBMacroSetTable class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBMacroSetTable
#define _H_CBMacroSetTable

#include <JXStringTable.h>
#include "CBPrefsManager.h"		// need definition of MacroSetInfo

class JXTextButton;
class CBCharActionTable;
class CBMacroTable;

class CBMacroSetTable : public JXStringTable
{
public:

	CBMacroSetTable(JArray<CBPrefsManager::MacroSetInfo>* macroList,
					const JIndex initialSelection, const JIndex firstUnusedID,
					CBCharActionTable* actionTable, CBMacroTable* macroTable,
					JXTextButton* addRowButton, JXTextButton* removeRowButton,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CBMacroSetTable();

	bool	ContentsValid() const;
	bool	GetCurrentMacroSetName(JString* name) const;

	JArray<CBPrefsManager::MacroSetInfo>*
		GetMacroList(JIndex* firstNewID, JIndex* lastNewID) const;

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

	JArray<CBPrefsManager::MacroSetInfo>*	itsMacroList;

	mutable bool	itsOwnsMacroListFlag;	// true => delete contents of itsMacroList
	const JIndex		itsFirstNewID;			// first index to use for new sets
	JIndex				itsLastNewID;			// index of last new set created
	JIndex				itsMacroIndex;			// index of currently displayed macro set

	CBCharActionTable*	itsActionTable;
	CBMacroTable*		itsMacroTable;

	JXTextButton*		itsAddRowButton;
	JXTextButton*		itsRemoveRowButton;

private:

	void	AddRow();
	void	RemoveRow();
	void	SwitchDisplay();

	// not allowed

	CBMacroSetTable(const CBMacroSetTable& source);
	const CBMacroSetTable& operator=(const CBMacroSetTable& source);
};

#endif
