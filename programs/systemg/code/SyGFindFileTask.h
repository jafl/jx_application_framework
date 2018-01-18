/******************************************************************************
 SyGFindFileTask.h

	Copyright @ 2008-09 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGFindFileTask
#define _H_SyGFindFileTask

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template; requires ace includes

class JProcess;
class SyGTreeDir;

class SyGFindFileTask : public JBroadcaster
{
public:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	RecordLink;

public:

	static JBoolean	Create(SyGTreeDir* dir, const JCharacter* path,
						   const JCharacter* expr, SyGFindFileTask** task);

	virtual ~SyGFindFileTask();

protected:

	SyGFindFileTask(SyGTreeDir* dir, const JCharacter* relPath,
					JProcess* p, int outFD, int errFD);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	SyGTreeDir*			itsDirector;		// not owned
	JPtrArray<JString>*	itsPathList;
	JProcess*			itsProcess;			// can be NULL
	RecordLink*			itsMessageLink;		// can be NULL
	RecordLink*			itsErrorLink;		// can be NULL
	JString				itsErrors;			// cache while process is running
	JBoolean			itsFoundFilesFlag;

private:

	void	ReceiveMessageLine();
	void	ReceiveErrorLine();
	void	DisplayErrors();

	static void	SplitPath(const JCharacter* origRelPath, JPtrArray<JString>* pathList);

	// not allowed

	SyGFindFileTask(const SyGFindFileTask& source);
	const SyGFindFileTask& operator=(const SyGFindFileTask& source);
};

#endif
