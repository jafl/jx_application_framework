/******************************************************************************
 JXFloatTable.h

	Interface for the JXFloatTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFloatTable
#define _H_JXFloatTable

#include "JXStyleTable.h"
#include <jx-af/jcore/JAuxTableData.h>
#include <jx-af/jcore/JFontStyle.h>

class JString;
class JFloatTableData;
class JFloatBufferTableData;
class JXFloatInput;

class JXFloatTable : public JXStyleTable
{
public:

	JXFloatTable(JFloatTableData* data, const int precision,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXFloatTable() override;

protected:

	JFloatTableData*	GetFloatData() const;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	virtual JXFloatInput*
		CreateFloatTableInput(const JPoint& cell, JXContainer* enclosure,
							  const HSizingOption hSizing, const VSizingOption vSizing,
							  const JCoordinate x, const JCoordinate y,
							  const JCoordinate w, const JCoordinate h);

	bool	GetInputField(JXFloatInput** widget) const;

private:

	JFloatTableData*		itsFloatData;			// we don't own this
	JFloatBufferTableData*	itsFloatBufferData;
	JXFloatInput*			itsFloatInputField;		// nullptr if not editing
};


/******************************************************************************
 GetFloatData (protected)

 ******************************************************************************/

inline JFloatTableData*
JXFloatTable::GetFloatData()
	const
{
	return itsFloatData;
}

/******************************************************************************
 GetInputField (protected)

 ******************************************************************************/

inline bool
JXFloatTable::GetInputField
	(
	JXFloatInput** widget
	)
	const
{
	*widget = itsFloatInputField;
	return *widget != nullptr;
}

#endif
