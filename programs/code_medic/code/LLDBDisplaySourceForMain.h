/******************************************************************************
 LLDBDisplaySourceForMain.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBDisplaySourceForMain
#define _H_LLDBDisplaySourceForMain

#include "CMDisplaySourceForMain.h"

class LLDBDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	LLDBDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~LLDBDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// not allowed

	LLDBDisplaySourceForMain(const LLDBDisplaySourceForMain& source);
	const LLDBDisplaySourceForMain& operator=(const LLDBDisplaySourceForMain& source);

};

#endif
