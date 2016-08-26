/******************************************************************************
 <PRE>MDIServer.h

	Copyright © <Year> by <Company>. All rights reserved.

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

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	// not allowed

	<PRE>MDIServer(const <PRE>MDIServer& source);
	const <PRE>MDIServer& operator=(const <PRE>MDIServer& source);
};

#endif
