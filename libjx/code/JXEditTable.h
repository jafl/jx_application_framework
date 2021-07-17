/******************************************************************************
 JXEditTable.h

	Interface for the JXEditTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEditTable
#define _H_JXEditTable

#include "JXTable.h"

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

	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;
	virtual bool	WantsInputFieldKey(const JUtf8Character& c, const int keySym,
										   const JXKeyModifiers& modifiers) const;

	JXTEBase*	GetEditMenuHandler() const;

	virtual bool	IsEditable(const JPoint& cell) const override;

protected:

	virtual bool	CreateInputField(const JPoint& cell, const JRect& cellRect) override;
	virtual void		DeleteInputField() override;
	virtual void		PlaceInputField(const JCoordinate x, const JCoordinate y) override;
	virtual void		MoveInputField(const JCoordinate dx, const JCoordinate dy) override;
	virtual void		SetInputFieldSize(const JCoordinate w, const JCoordinate h) override;
	virtual void		ResizeInputField(const JCoordinate dw, const JCoordinate dh) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) = 0;
	virtual void			PrepareDeleteXInputField() = 0;
	bool				GetXInputField(JXInputField** inputField) const;

	virtual JCoordinate	GetMin1DVisibleWidth(const JPoint& cell) const override;

private:

	JXInputField*	itsInputField;		// used for edit-in-place; can be nullptr; not owned
	JXInputField*	itsEditMenuHandler;	// maintains Edit menu; can be nullptr

private:

	// not allowed

	JXEditTable(const JXEditTable& source);
	const JXEditTable& operator=(const JXEditTable& source);
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
