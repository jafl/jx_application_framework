/******************************************************************************
 SVNTextBase.h

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNTextBase
#define _H_SVNTextBase

#include <JXStaticText.h>
#include "SVNTabBase.h"
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JAsynchDataReceiver.h>

class JProcess;

class SVNTextBase : public JXStaticText, public SVNTabBase
{
public:

	typedef JAsynchDataReceiver<ACE_LSOCK_STREAM>	DataLink;

public:

	SVNTextBase(SVNMainDirector* director, JXTextMenu* editMenu,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~SVNTextBase();

	virtual void	UpdateActionsMenu(JXTextMenu* menu);
	virtual void	UpdateInfoMenu(JXTextMenu* menu);
	virtual void	RefreshContent();

protected:

	virtual JError	StartProcess(JProcess** p, int* outFD) = 0;

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JProcess*		itsProcess;		// can be NULL
	DataLink*		itsLink;		// can be NULL

private:

	void	SetConnection(JProcess* p, const int fd);
	void	DeleteLink();
	void	ReceiveData(const Message& message);

	// not allowed

	SVNTextBase(const SVNTextBase& source);
	const SVNTextBase& operator=(const SVNTextBase& source);
};

#endif
