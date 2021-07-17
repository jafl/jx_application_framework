/******************************************************************************
 SyGFindFileTask.h

	Copyright @ 2008-09 by John Lindal.

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

	static bool	Create(SyGTreeDir* dir, const JString& path,
						   const JString& expr, SyGFindFileTask** task);

	virtual ~SyGFindFileTask();

protected:

	SyGFindFileTask(SyGTreeDir* dir, const JString& relPath,
					JProcess* p, int outFD, int errFD);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	SyGTreeDir*			itsDirector;		// not owned
	JPtrArray<JString>*	itsPathList;
	JProcess*			itsProcess;			// can be nullptr
	RecordLink*			itsMessageLink;		// can be nullptr
	RecordLink*			itsErrorLink;		// can be nullptr
	JString				itsErrors;			// cache while process is running
	bool			itsFoundFilesFlag;

private:

	void	ReceiveMessageLine();
	void	ReceiveErrorLine();
	void	DisplayErrors();

	static void	SplitPath(const JString& origRelPath, JPtrArray<JString>* pathList);

	// not allowed

	SyGFindFileTask(const SyGFindFileTask& source);
	const SyGFindFileTask& operator=(const SyGFindFileTask& source);
};

#endif
