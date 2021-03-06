/******************************************************************************
 JFloatTableData.h

	Interface for the JFloatTableData class

	Copyright (C) 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFloatTableData
#define _H_JFloatTableData

#include <JValueTableData.h>

class JFloatTableData : public JValueTableData<JArray<JFloat>,JFloat>
{
public:

	JFloatTableData(const JFloat defValue);

	virtual ~JFloatTableData();
};

#endif
