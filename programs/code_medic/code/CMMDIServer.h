/******************************************************************************
 CMMDIServer.h

	Interface for the CMMDIServer class

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_CMMDIServer
#define _H_CMMDIServer

#include <JXMDIServer.h>

class CMMDIServer : public JXMDIServer, virtual public JBroadcaster
{
public:

	CMMDIServer();

	virtual	~CMMDIServer();

	static void	UpdateDebuggerType(const JString& program);
	static JBoolean	IsBinary(const JString& fileName);
	static JBoolean	GetLanguage(const JString& fileName, JString* language);

protected:

	virtual void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;

private:

	JBoolean	itsFirstTimeFlag;

private:

	// not allowed

	CMMDIServer(const CMMDIServer& source);
	const CMMDIServer& operator=(const CMMDIServer& source);
};

#endif
