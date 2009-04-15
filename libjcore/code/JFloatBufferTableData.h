/******************************************************************************
 JFloatBufferTableData.h

	Interface for the JFloatBufferTableData Class

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFloatBufferTableData
#define _H_JFloatBufferTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JStringTableData.h>

class JFloatTableData;

class JFloatBufferTableData : public JStringTableData
{
public:

	JFloatBufferTableData(const JFloatTableData* floatData,
						  const int precision);

	virtual ~JFloatBufferTableData();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const JFloatTableData*	itsFloatData;
	const int				itsPrecision;

private:

	void	UpdateCell(const JPoint& cell);
	void	UpdateRows(const JIndex firstIndex, const JSize count);
	void	UpdateCols(const JIndex firstIndex, const JSize count);
	void	UpdateRect(const JRect& r);

	// not allowed

	JFloatBufferTableData(const JFloatBufferTableData& source);
	const JFloatBufferTableData& operator=(const JFloatBufferTableData& source);
};


/******************************************************************************
 UpdateRows (private)

 ******************************************************************************/

inline void
JFloatBufferTableData::UpdateRows
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	UpdateRect(JRect(firstIndex, 1, firstIndex+count, GetColCount()+1));
}

/******************************************************************************
 UpdateCols (private)

 ******************************************************************************/

inline void
JFloatBufferTableData::UpdateCols
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	UpdateRect(JRect(1, firstIndex, GetRowCount()+1, firstIndex+count));
}

#endif
