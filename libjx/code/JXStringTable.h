/******************************************************************************
 JXStringTable.h

	Interface for the JXStringTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStringTable
#define _H_JXStringTable

#include "jx-af/jx/JXStyleTable.h"
#include <jx-af/jcore/JAuxTableData.h>
#include <jx-af/jcore/JFontStyle.h>

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

	~JXStringTable();

protected:

	JStringTableData*	GetStringData() const;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h);

	bool	GetInputField(JXInputField** widget) const;

private:

	JStringTableData*	itsStringData;			// we don't own this
	JXInputField*		itsStringInputField;	// nullptr if not editing
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

inline bool
JXStringTable::GetInputField
	(
	JXInputField** widget
	)
	const
{
	*widget = itsStringInputField;
	return *widget != nullptr;
}

#endif
