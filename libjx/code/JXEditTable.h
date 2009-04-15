/******************************************************************************
 JXEditTable.h

	Interface for the JXEditTable class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXEditTable
#define _H_JXEditTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTable.h>

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

	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual JBoolean	WantsInputFieldKey(const int key, const JXKeyModifiers& modifiers) const;

	JXTEBase*	GetEditMenuHandler() const;

	virtual JBoolean	IsEditable(const JPoint& cell) const;

protected:

	virtual JBoolean	CreateInputField(const JPoint& cell, const JRect& cellRect);
	virtual void		DeleteInputField();
	virtual void		PlaceInputField(const JCoordinate x, const JCoordinate y);
	virtual void		MoveInputField(const JCoordinate dx, const JCoordinate dy);
	virtual void		SetInputFieldSize(const JCoordinate w, const JCoordinate h);
	virtual void		ResizeInputField(const JCoordinate dw, const JCoordinate dh);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) = 0;
	virtual void			PrepareDeleteXInputField() = 0;
	JBoolean				GetXInputField(JXInputField** inputField) const;

	virtual JCoordinate	GetMin1DVisibleWidth(const JPoint& cell) const;

private:

	JXInputField*	itsInputField;		// used for edit-in-place; can be NULL; not owned
	JXInputField*	itsEditMenuHandler;	// maintains Edit menu; can be NULL

private:

	// not allowed

	JXEditTable(const JXEditTable& source);
	const JXEditTable& operator=(const JXEditTable& source);
};


/******************************************************************************
 GetXInputField (protected)

	Returns kJTrue if we have an active input field.

 ******************************************************************************/

inline JBoolean
JXEditTable::GetXInputField
	(
	JXInputField** inputField
	)
	const
{
	*inputField = itsInputField;
	return JConvertToBoolean( itsInputField != NULL );
}

#endif
