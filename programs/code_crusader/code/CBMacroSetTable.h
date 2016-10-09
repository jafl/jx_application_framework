/******************************************************************************
 CBMacroSetTable.h

	Interface for the CBMacroSetTable class

	Copyright (C) 1998 by John Lindal. All rights reserved.

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

	JBoolean	ContentsValid() const;
	JBoolean	GetCurrentMacroSetName(JString* name) const;

	JArray<CBPrefsManager::MacroSetInfo>*
		GetMacroList(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JArray<CBPrefsManager::MacroSetInfo>*	itsMacroList;

	mutable JBoolean	itsOwnsMacroListFlag;	// kJTrue => jdelete contents of itsMacroList
	const JIndex		itsFirstNewID;			// first index to use for jnew sets
	JIndex				itsLastNewID;			// index of last jnew set created
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
