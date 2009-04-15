/******************************************************************************
 JXStringTable.h

	Interface for the JXStringTable class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStringTable
#define _H_JXStringTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStyleTable.h>
#include <JAuxTableData.h>
#include <JFontStyle.h>

class JString;
class JStringTableData;
class JXInputField;

class JXStringTable : public JXStyleTable
{
public:

	JXStringTable(JStringTableData* data,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~JXStringTable();

protected:

	JStringTableData*	GetStringData() const;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h);

	JBoolean	GetInputField(JXInputField** widget) const;

private:

	JStringTableData*	itsStringData;			// we don't own this
	JXInputField*		itsStringInputField;	// NULL if not editing

private:

	// not allowed

	JXStringTable(const JXStringTable& source);
	const JXStringTable& operator=(const JXStringTable& source);
};


/******************************************************************************
 GetStringData (protected)

 ******************************************************************************/

inline JStringTableData*
JXStringTable::GetStringData()
	const
{
	return itsStringData;
}

/******************************************************************************
 GetInputField (protected)

 ******************************************************************************/

inline JBoolean
JXStringTable::GetInputField
	(
	JXInputField** widget
	)
	const
{
	*widget = itsStringInputField;
	return JI2B(*widget != NULL);
}

#endif
