/******************************************************************************
 JXEditTable.h

	Interface for the JXEditTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEditTable
#define _H_JXEditTable

#include "jx-af/jx/JXTable.h"

class JXTEBase;
class JXInputField;

class JXEditTable : public JXTable
{
public:

	JXEditTable(const JCoordinate defRowHeight, const JCoordinate defColWidth,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXEditTable();

	void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;
	virtual bool	WantsInputFieldKey(const JUtf8Character& c, const int keySym,
										   const JXKeyModifiers& modifiers) const;

	JXTEBase*	GetEditMenuHandler() const;

	bool	IsEditable(const JPoint& cell) const override;

protected:

	bool	CreateInputField(const JPoint& cell, const JRect& cellRect) override;
	void		DeleteInputField() override;
	void		PlaceInputField(const JCoordinate x, const JCoordinate y) override;
	void		MoveInputField(const JCoordinate dx, const JCoordinate dy) override;
	void		SetInputFieldSize(const JCoordinate w, const JCoordinate h) override;
	void		ResizeInputField(const JCoordinate dw, const JCoordinate dh) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) = 0;
	virtual void			PrepareDeleteXInputField() = 0;
	bool				GetXInputField(JXInputField** inputField) const;

	JCoordinate	GetMin1DVisibleWidth(const JPoint& cell) const override;

private:

	JXInputField*	itsInputField;		// used for edit-in-place; can be nullptr; not owned
	JXInputField*	itsEditMenuHandler;	// maintains Edit menu; can be nullptr
};


/******************************************************************************
 GetXInputField (protected)

	Returns true if we have an active input field.

 ******************************************************************************/

inline bool
JXEditTable::GetXInputField
	(
	JXInputField** inputField
	)
	const
{
	*inputField = itsInputField;
	return itsInputField != nullptr;
}

#endif
