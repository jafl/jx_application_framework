/******************************************************************************
 GNBMDIServer.h

	Interface for the GNBMDIServer class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GNBMDIServer
#define _H_GNBMDIServer

#include <JXMDIServer.h>

class GNBApp;

class GNBMDIServer : public JXMDIServer
{
public:

	GNBMDIServer(GNBApp* app);
	virtual ~GNBMDIServer();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

//	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	GNBApp*	itsApp;

private:

	// not allowed

	GNBMDIServer(const GNBMDIServer& source);
	const GNBMDIServer& operator=(const GNBMDIServer& source);

};

#endif
