/******************************************************************************
 JXIntegerTable.h

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXIntegerTable
#define _H_JXIntegerTable

#include "JXStyleTable.h"
#include <jx-af/jcore/JAuxTableData.h>
#include <jx-af/jcore/JFontStyle.h>

class JString;
class JIntegerTableData;
class JIntegerBufferTableData;
class JXIntegerInput;

class JXIntegerTable : public JXStyleTable
{
public:

	JXIntegerTable(JIntegerTableData* data,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~JXIntegerTable() override;

protected:

	JIntegerTableData*	GetIntegerData() const;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	virtual JXIntegerInput*
		CreateIntegerTableInput(const JPoint& cell, JXContainer* enclosure,
								const HSizingOption hSizing, const VSizingOption vSizing,
								const JCoordinate x, const JCoordinate y,
								const JCoordinate w, const JCoordinate h);

	bool	GetInputField(JXIntegerInput** widget) const;

private:

	JIntegerTableData*			itsIntegerData;			// we don't own this
	JIntegerBufferTableData*	itsIntegerBufferData;
	JXIntegerInput*				itsIntegerInputField;		// nullptr if not editing
};


/******************************************************************************
 GetIntegerData (protected)

 ******************************************************************************/

inline JIntegerTableData*
JXIntegerTable::GetIntegerData()
	const
{
	return itsIntegerData;
}

/******************************************************************************
 GetInputField (protected)

 ******************************************************************************/

inline bool
JXIntegerTable::GetInputField
	(
	JXIntegerInput** widget
	)
	const
{
	*widget = itsIntegerInputField;
	return *widget != nullptr;
}

#endif
