/******************************************************************************
 JFloatTableData.h

	Interface for the JFloatTableData class

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JFloatTableData
#define _H_JFloatTableData

#include "jx-af/jcore/JValueTableData.h"

class JFloatTableData : public JValueTableData<JArray<JFloat>,JFloat>
{
public:

	JFloatTableData(const JFloat defValue);

	~JFloatTableData() override;
};

#endif
