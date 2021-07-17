/******************************************************************************
 GDBDisplaySourceForMain.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBDisplaySourceForMain
#define _H_GDBDisplaySourceForMain

#include "CMDisplaySourceForMain.h"

class GDBDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	GDBDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~GDBDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsHasCoreFlag;
	JIndex		itsNextCmdIndex;

private:

	// not allowed

	GDBDisplaySourceForMain(const GDBDisplaySourceForMain& source);
	const GDBDisplaySourceForMain& operator=(const GDBDisplaySourceForMain& source);

};

#endif
