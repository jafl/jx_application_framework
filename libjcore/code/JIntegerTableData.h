/******************************************************************************
 JIntegerTableData.h

	Copyright (C) 2024 John Lindal.

 ******************************************************************************/

#ifndef _H_JIntegerTableData
#define _H_JIntegerTableData

#include "JValueTableData.h"

class JIntegerTableData : public JValueTableData<JArray<JInteger>,JInteger>
{
public:

	JIntegerTableData(const JInteger defValue);

	~JIntegerTableData() override;
};

#endif
