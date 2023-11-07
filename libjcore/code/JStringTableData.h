/******************************************************************************
 JStringTableData.h

	Interface for the JStringTableData class

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JStringTableData
#define _H_JStringTableData

#include "JObjTableData.h"

class JString;

class JStringTableData : public JObjTableData<JString>
{
public:

	JStringTableData();
	JStringTableData(const JStringTableData& source);

	~JStringTableData() override;

	const JString&	GetString(const JIndex row, const JIndex col) const;
	const JString&	GetString(const JPoint& cell) const;

	void	SetString(const JIndex row, const JIndex col, const JString& s);
	void	SetString(const JPoint& cell, const JString& s);
};

/******************************************************************************
 GetString

 ******************************************************************************/

inline const JString&
JStringTableData::GetString
	(
	const JIndex row,
	const JIndex col
	)
	const
{
	return GetItem(row,col);
}

inline const JString&
JStringTableData::GetString
	(
	const JPoint& cell
	)
	const
{
	return GetItem(cell);
}

/******************************************************************************
 SetString

 ******************************************************************************/

inline void
JStringTableData::SetString
	(
	const JIndex	row,
	const JIndex	col,
	const JString&	s
	)
{
	SetItem(row,col, s);
}

inline void
JStringTableData::SetString
	(
	const JPoint&	cell,
	const JString&	s
	)
{
	SetItem(cell, s);
}

#endif
