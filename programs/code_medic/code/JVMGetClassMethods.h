/******************************************************************************
 JVMGetClassMethods.h

	Copyright © 2011 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetClassMethods
#define _H_JVMGetClassMethods

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class JVMGetClassMethods : public CMCommand
{
public:

	JVMGetClassMethods(const JUInt64 id);

	virtual	~JVMGetClassMethods();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	const JUInt64	itsID;

private:

	// not allowed

	JVMGetClassMethods(const JVMGetClassMethods& source);
	const JVMGetClassMethods& operator=(const JVMGetClassMethods& source);

};

#endif
