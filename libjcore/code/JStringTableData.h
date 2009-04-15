/******************************************************************************
 JStringTableData.h

	Interface for the JStringTableData class

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStringTableData
#define _H_JStringTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JObjTableData.h>

class JString;

class JStringTableData : public JObjTableData<JString>
{
public:

	JStringTableData();
	JStringTableData(const JStringTableData& source);

	virtual ~JStringTableData();

	const JString&	GetString(const JIndex row, const JIndex col) const;
	const JString&	GetString(const JPoint& cell) const;

	void	SetString(const JIndex row, const JIndex col, const JCharacter* str);
	void	SetString(const JPoint& cell, const JCharacter* str);

	void	SetString(const JIndex row, const JIndex col, const JString& s);
	void	SetString(const JPoint& cell, const JString& s);

private:

	// not allowed

	const JStringTableData& operator=(const JStringTableData& source);
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
	return GetElement(row,col);
}

inline const JString&
JStringTableData::GetString
	(
	const JPoint& cell
	)
	const
{
	return GetElement(cell);
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
	SetElement(row,col, s);
}

inline void
JStringTableData::SetString
	(
	const JPoint&	cell,
	const JString&	s
	)
{
	SetElement(cell, s);
}

#endif
