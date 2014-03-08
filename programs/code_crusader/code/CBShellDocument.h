/******************************************************************************
 CBShellDocument.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBShellDocument
#define _H_CBShellDocument

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBTextDocument.h"
#include <JProcess.h>
#include <j_prep_ace.h>
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

	static JBoolean	Create(CBShellDocument** doc);

	virtual ~CBShellDocument();

	JBoolean	ProcessRunning() const;
	void		SendCommand(const JCharacter* cmd);

protected:

	CBShellDocument(JProcess* p, const int inFD, const int outFD);

	void	KillProcess();

	JBoolean	GetDataLink(DataLink** link) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
												 const JCharacter* fileName,
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

inline JBoolean
CBShellDocument::ProcessRunning()
	const
{
	return JI2B( itsProcess != NULL && !itsProcess->IsFinished() );
}

/******************************************************************************
 GetDataLink (protected)

 ******************************************************************************/

inline JBoolean
CBShellDocument::GetDataLink
	(
	DataLink** link
	)
	const
{
	*link = itsDataLink;
	return JI2B( itsDataLink != NULL );
}

#endif
