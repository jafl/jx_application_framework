/******************************************************************************
 JXFloatTable.h

	Interface for the JXFloatTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFloatTable
#define _H_JXFloatTable

#include <JXStyleTable.h>
#include <JAuxTableData.h>
#include <JFontStyle.h>

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

	virtual ~JXFloatTable();

protected:

	JFloatTableData*	GetFloatData() const;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual JXFloatInput*
		CreateFloatTableInput(const JPoint& cell, JXContainer* enclosure,
							  const HSizingOption hSizing, const VSizingOption vSizing,
							  const JCoordinate x, const JCoordinate y,
							  const JCoordinate w, const JCoordinate h);

	JBoolean	GetInputField(JXFloatInput** widget) const;

private:

	JFloatTableData*		itsFloatData;			// we don't own this
	JFloatBufferTableData*	itsFloatBufferData;
	JXFloatInput*			itsFloatInputField;		// nullptr if not editing

private:

	// not allowed

	JXFloatTable(const JXFloatTable& source);
	const JXFloatTable& operator=(const JXFloatTable& source);
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

inline JBoolean
JXFloatTable::GetInputField
	(
	JXFloatInput** widget
	)
	const
{
	*widget = itsFloatInputField;
	return JI2B(*widget != nullptr);
}

#endif
