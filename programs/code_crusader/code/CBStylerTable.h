/******************************************************************************
 CBStylerTable.h

	Interface for the CBStylerTable class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBStylerTable
#define _H_CBStylerTable

#include <JXStringTable.h>
#include <JStringMap.h>
#include "CBStylerBase.h"		// need definition of WordStyle

class JXTextButton;
class CBStylerTableMenu;

class CBStylerTable : public JXStringTable
{
public:

	CBStylerTable(const JCharacter** typeNames,
				  const JArray<JFontStyle>& typeStyles,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	CBStylerTable(const CBTextFileType fileType,
				  const JArray<CBStylerBase::WordStyle>& wordList,
				  JXTextButton* addRowButton, JXTextButton* removeButton,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~CBStylerTable();

	void	GetData(JArray<JFontStyle>* typeStyles) const;
	void	GetData(JStringMap<JFontStyle>* wordStyles) const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	// called by CBStylerTableInput

	void	DisplayFontMenu(const JPoint& cell, JXContainer* mouseOwner,
							const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
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

	CBTextFileType		itsFileType;
	JBoolean			itsAllowEditFlag;
	CBStylerTableMenu*	itsStyleMenu;
	JXTextButton*		itsAddRowButton;	// can be NULL
	JXTextButton*		itsRemoveButton;	// can be NULL

private:

	void	CBStylerTableX(const CBTextFileType fileType, const JBoolean allowEdit,
						   JXTextButton* addRowButton, JXTextButton* removeButton);
	void	UpdateButtons();

	void	AddRow();
	void	RemoveSelection();

	// not allowed

	CBStylerTable(const CBStylerTable& source);
	const CBStylerTable& operator=(const CBStylerTable& source);
};

#endif
