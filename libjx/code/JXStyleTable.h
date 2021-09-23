/******************************************************************************
 JXStyleTable.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStyleTable
#define _H_JXStyleTable

#include "JXEditTable.h"
#include <JFont.h>

class JString;
class JStyleTableData;

class JXStyleTable : public JXEditTable
{
public:

	JXStyleTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXStyleTable();

	const JFont&	GetFont() const;
	void			SetFont(const JString& name, const JSize size);

	JFontStyle	GetCellStyle(const JPoint& cell) const;
	void		SetCellStyle(const JPoint& cell, const JFontStyle& style);
	void		SetAllCellStyles(const JFontStyle& style);

	const JStyleTableData&	GetStyleData() const;

protected:

	virtual JCoordinate	GetMin1DVisibleWidth(const JPoint& cell) const;

private:

	JStyleTableData*	itsStyleData;
};


/******************************************************************************
 GetStyleData

 ******************************************************************************/

inline const JStyleTableData&
JXStyleTable::GetStyleData()
	const
{
	return *itsStyleData;
}

#endif
