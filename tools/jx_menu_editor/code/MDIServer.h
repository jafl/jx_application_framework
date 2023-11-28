/******************************************************************************
 MDIServer.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_MDIServer
#define _H_MDIServer

#include <jx-af/jx/JXMDIServer.h>

class MDIServer : public JXMDIServer
{
public:

	MDIServer();

	~MDIServer() override;

	static bool	ChooseFiles();

protected:

	void	HandleMDIRequest(const JString& dir,
							 const JPtrArray<JString>& argList) override;
};

#endif
