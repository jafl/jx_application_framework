/******************************************************************************
 JVMDisplaySourceForMain.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMDisplaySourceForMain
#define _H_JVMDisplaySourceForMain

#include "CMDisplaySourceForMain.h"

class JVMDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	JVMDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~JVMDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMDisplaySourceForMain(const JVMDisplaySourceForMain& source);
	const JVMDisplaySourceForMain& operator=(const JVMDisplaySourceForMain& source);

};

#endif
