/******************************************************************************
 CMMDIServer.h

	Interface for the CMMDIServer class

	Copyright © 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMMDIServer
#define _H_CMMDIServer

#include <JXMDIServer.h>

class CMMDIServer : public JXMDIServer, virtual public JBroadcaster
{
public:

	CMMDIServer();

	virtual	~CMMDIServer();

	static void	UpdateDebuggerType(const JCharacter* program);
	static JBoolean	IsBinary(const JCharacter* fileName);
	static JBoolean	GetLanguage(const JCharacter* fileName, JString* language);

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	JBoolean	itsFirstTimeFlag;

private:

	// not allowed

	CMMDIServer(const CMMDIServer& source);
	const CMMDIServer& operator=(const CMMDIServer& source);
};

#endif
