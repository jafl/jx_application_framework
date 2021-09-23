/******************************************************************************
 JFloatBufferTableData.h

	Interface for the JFloatBufferTableData Class

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JFloatBufferTableData
#define _H_JFloatBufferTableData

#include "JStringTableData.h"

class JFloatTableData;

class JFloatBufferTableData : public JStringTableData
{
public:

	JFloatBufferTableData(const JFloatTableData* floatData,
						  const int precision);

	virtual ~JFloatBufferTableData();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JFloatTableData*	itsFloatData;
	const int				itsPrecision;

private:

	void	UpdateCell(const JPoint& cell);
	void	UpdateRows(const JIndex firstIndex, const JSize count);
	void	UpdateCols(const JIndex firstIndex, const JSize count);
	void	UpdateRect(const JRect& r);

	// not allowed

	JFloatBufferTableData(const JFloatBufferTableData&) = delete;
};

#endif
