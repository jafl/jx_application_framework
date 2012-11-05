/******************************************************************************
 GDBGetAssembly.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetAssembly
#define _H_GDBGetAssembly

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetAssembly.h"

class CMSourceDirector;

class GDBGetAssembly : public CMGetAssembly
{
public:

	GDBGetAssembly(CMSourceDirector* dir);

	virtual	~GDBGetAssembly();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBGetAssembly(const GDBGetAssembly& source);
	const GDBGetAssembly& operator=(const GDBGetAssembly& source);

};

#endif
