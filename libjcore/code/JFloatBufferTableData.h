/******************************************************************************
 JFloatBufferTableData.h

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JFloatBufferTableData
#define _H_JFloatBufferTableData

#include "JBufferTableData.h"

class JFloatTableData;

class JFloatBufferTableData : public JBufferTableData
{
public:

	JFloatBufferTableData(const JFloatTableData* floatData,
						  const int precision);

	~JFloatBufferTableData() override;

protected:

	void	UpdateCell(const JPoint& cell) override;

private:

	const JFloatTableData*	itsFloatData;
	const int				itsPrecision;

private:

	// not allowed

	JFloatBufferTableData(const JFloatBufferTableData&) = delete;
};

#endif
