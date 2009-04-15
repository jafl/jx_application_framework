/******************************************************************************
 JFloatTableData.h

	Interface for the JFloatTableData class

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFloatTableData
#define _H_JFloatTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JValueTableData.h>

class JFloatTableData : public JValueTableData<JArray<JFloat>,JFloat>
{
public:

	JFloatTableData(const JFloat defValue);

	virtual ~JFloatTableData();
};

#endif
