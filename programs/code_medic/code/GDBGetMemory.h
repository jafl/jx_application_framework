/******************************************************************************
 GDBGetMemory.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetMemory
#define _H_GDBGetMemory

#include "CMGetMemory.h"

class GDBGetMemory : public CMGetMemory
{
public:

	GDBGetMemory(CMMemoryDir* dir);

	virtual	~GDBGetMemory();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBGetMemory(const GDBGetMemory& source);
	const GDBGetMemory& operator=(const GDBGetMemory& source);
};

#endif
