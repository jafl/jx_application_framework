/******************************************************************************
 CBShellDocument.h

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBShellDocument
#define _H_CBShellDocument

#include "CBTextDocument.h"
#include <JProcess.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JAsynchDataReceiver.h>

class JOutPipeStream;
class CBShellEditor;

class CBShellDocument : public CBTextDocument
{
public:

	typedef JAsynchDataReceiver<ACE_LSOCK_STREAM>	DataLink;

public:

	static bool	Create(CBShellDocument** doc);

	virtual ~CBShellDocument();

	bool	ProcessRunning() const;
	void		SendCommand(const JString& cmd);

protected:

	CBShellDocument(JProcess* p, const int inFD, const int outFD);

	void	KillProcess();

	bool	GetDataLink(DataLink** link) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBShellEditor*	itsShellEditor;
	JProcess*		itsProcess;
	DataLink*		itsDataLink;
	JOutPipeStream*	itsCmdStream;

	JXTextButton*	itsKillButton;

private:

	void	SetConnection(JProcess* p, const int inFD, const int outFD);
	void	ReceiveData(const Message& message);
	void	UpdateButtons();
	void	DeleteLinks();
	void	CloseOutFD();

	static CBTextEditor*	ConstructShellEditor(CBTextDocument* document,
												 const JString& fileName,
												 JXMenuBar* menuBar,
												 CBTELineIndexInput* lineInput,
												 CBTEColIndexInput* colInput,
												 JXScrollbarSet* scrollbarSet);

	// not allowed

	CBShellDocument(const CBShellDocument& source);
	const CBShellDocument& operator=(const CBShellDocument& source);
};


/******************************************************************************
 ProcessRunning

 ******************************************************************************/

inline bool
CBShellDocument::ProcessRunning()
	const
{
	return itsProcess != nullptr && !itsProcess->IsFinished();
}

/******************************************************************************
 GetDataLink (protected)

 ******************************************************************************/

inline bool
CBShellDocument::GetDataLink
	(
	DataLink** link
	)
	const
{
	*link = itsDataLink;
	return itsDataLink != nullptr;
}

#endif
