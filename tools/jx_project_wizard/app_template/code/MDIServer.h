/******************************************************************************
 MDIServer.h

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_MDIServer
#define _H_MDIServer

#include <jx-af/jx/JXMDIServer.h>

class MDIServer : public JXMDIServer
{
public:

	MDIServer();

	virtual ~MDIServer();

protected:

	void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;
};

#endif
