/******************************************************************************
 JXStyleTable.h

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStyleTable
#define _H_JXStyleTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>
#include <JFontStyle.h>

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

	const JString&	GetFont(JSize* size) const;
	void			SetFont(const JCharacter* name, const JSize size);

	JFontStyle	GetCellStyle(const JPoint& cell) const;
	void		SetCellStyle(const JPoint& cell, const JFontStyle& style);
	void		SetAllCellStyles(const JFontStyle& style);

	const JStyleTableData&	GetStyleData() const;

protected:

	virtual JCoordinate	GetMin1DVisibleWidth(const JPoint& cell) const;

private:

	JStyleTableData*	itsStyleData;

private:

	// not allowed

	JXStyleTable(const JXStyleTable& source);
	const JXStyleTable& operator=(const JXStyleTable& source);
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
