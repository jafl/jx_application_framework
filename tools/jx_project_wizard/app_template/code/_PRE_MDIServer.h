/******************************************************************************
 <PRE>MDIServer.h

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_<PRE>MDIServer
#define _H_<PRE>MDIServer

#include <JXMDIServer.h>

class <PRE>MDIServer : public JXMDIServer
{
public:

	<PRE>MDIServer();

	virtual	~<PRE>MDIServer();

protected:

	virtual void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;
};

#endif
