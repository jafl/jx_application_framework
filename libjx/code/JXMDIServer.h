/******************************************************************************
 JXMDIServer.h

	Interface for the JXMDIServer class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMDIServer
#define _H_JXMDIServer

#include <jx-af/jcore/JMDIServer.h>

class JXMDIServer : public JMDIServer
{
public:

	JXMDIServer();

	~JXMDIServer() override;

protected:

	bool	CanAcceptMDIRequest() override;
	void	PreprocessArgList(JPtrArray<JString>* argList) override;
};

#endif
