/******************************************************************************
 JBufferTableData.h

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JBufferTableData
#define _H_JBufferTableData

#include "JStringTableData.h"

class JFloatTableData;

class JBufferTableData : public JStringTableData
{
public:

	JBufferTableData(const JTableData* data);

	~JBufferTableData() override;

protected:

	void			UpdateRect(const JRect& r);
	void			UpdateRows(const JIndex firstIndex, const JSize count);
	void			UpdateCols(const JIndex firstIndex, const JSize count);
	virtual void	UpdateCell(const JPoint& cell) = 0;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JTableData*	itsData;

private:

	// not allowed

	JBufferTableData(const JBufferTableData&) = delete;
};

#endif
