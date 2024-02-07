/******************************************************************************
 JIntegerBufferTableData.h

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JIntegerBufferTableData
#define _H_JIntegerBufferTableData

#include "JBufferTableData.h"

class JIntegerTableData;

class JIntegerBufferTableData : public JBufferTableData
{
public:

	JIntegerBufferTableData(const JIntegerTableData* intData);

	~JIntegerBufferTableData() override;

protected:

	void	UpdateCell(const JPoint& cell) override;

private:

	const JIntegerTableData*	itsIntegerData;

private:

	// not allowed

	JIntegerBufferTableData(const JIntegerBufferTableData&) = delete;
};

#endif
