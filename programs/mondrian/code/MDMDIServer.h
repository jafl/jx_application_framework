/******************************************************************************
 MDMDIServer.h

	Copyright (C) 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_MDMDIServer
#define _H_MDMDIServer

#include <JXMDIServer.h>

class MDMDIServer : public JXMDIServer
{
public:

	MDMDIServer();

	virtual	~MDMDIServer();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	// not allowed

	MDMDIServer(const MDMDIServer& source);
	const MDMDIServer& operator=(const MDMDIServer& source);
};

#endif
