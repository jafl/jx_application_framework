/******************************************************************************
 GInboxMgr.h

	Interface for the GInboxMgr class

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GInboxMgr
#define _H_GInboxMgr

#include <JPtrArray-JString.h>

class JDirEntry;
class JXTimerTask;

class GInboxMgr : virtual public JBroadcaster
{
public:

	GInboxMgr();
	virtual ~GInboxMgr();

	void	SaveState(std::ostream& os);
	void	ReadState(std::istream& is);
	void	MailboxClosed(const JString& mailbox);
	void	ShouldBePostponingUpdate(const JBoolean postpone);

protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);

private:

	JPtrArray<JDirEntry>*	itsEntries;
	JArray<time_t>*			itsTimes;
	JXTimerTask*			itsTimer;
	JBoolean				itsPostponeUpdate;

private:

	void	UpdateEntries();
	void	CheckEntries();

	// not allowed

	GInboxMgr(const GInboxMgr& source);
	const GInboxMgr& operator=(const GInboxMgr& source);

public:

};


#endif
