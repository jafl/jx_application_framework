/******************************************************************************
 CBStylerTable.h

	Interface for the CBStylerTable class

	Copyright © 1998 by John Lindal.

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

	CBStylerTable(const JUtf8Byte** typeNames,
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

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

	// called by CBStylerTableInput

	void	DisplayFontMenu(const JPoint& cell, JXContainer* mouseOwner,
							const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
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

	CBTextFileType		itsFileType;
	bool				itsAllowEditFlag;
	CBStylerTableMenu*	itsStyleMenu;
	JXTextButton*		itsAddRowButton;	// can be nullptr
	JXTextButton*		itsRemoveButton;	// can be nullptr

private:

	void	CBStylerTableX(const CBTextFileType fileType, const bool allowEdit,
						   JXTextButton* addRowButton, JXTextButton* removeButton);
	void	UpdateButtons();

	void	AddRow();
	void	RemoveSelection();

	// not allowed

	CBStylerTable(const CBStylerTable& source);
	const CBStylerTable& operator=(const CBStylerTable& source);
};

#endif
