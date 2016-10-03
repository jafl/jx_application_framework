/******************************************************************************
 CBCPPMacroTable.h

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCPPMacroTable
#define _H_CBCPPMacroTable

#include <JXStringTable.h>

class JXTextButton;
class JXColHeaderWidget;
class CBCPreprocessor;
class CBListCSF;

class CBCPPMacroTable : public JXStringTable
{
public:

	CBCPPMacroTable(const CBCPreprocessor& cpp,
					JXTextButton* addRowButton, JXTextButton* removeRowButton,
					JXTextButton* loadButton, JXTextButton* saveButton,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CBCPPMacroTable();

	JBoolean	ContentsValid() const;
	JBoolean	UpdateMacros(CBCPreprocessor* cpp) const;

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;
	void	SetColTitles(JXColHeaderWidget* widget) const;

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

	virtual void	PrepareDeleteXInputField();
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextButton*	itsAddRowButton;
	JXTextButton*	itsRemoveRowButton;

	CBListCSF*		itsCSF;
	mutable JString	itsFileName;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

private:

	void	AddRow();
	void	RemoveRow();

	void	LoadMacros();
	void	SaveMacros() const;

	void	ReadData(const JCharacter* fileName, const JBoolean replace);
	void	WriteData(const JCharacter* fileName) const;

	void	AdjustColWidths();

	// not allowed

	CBCPPMacroTable(const CBCPPMacroTable& source);
	const CBCPPMacroTable& operator=(const CBCPPMacroTable& source);
};

#endif
