/******************************************************************************
 CBExecOutputDocument.h

	Interface for the CBExecOutputDocument class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBExecOutputDocument
#define _H_CBExecOutputDocument

#include "CBTextDocument.h"
#include <JProcess.h>			// need definition of JProcess::Finished
#include <j_prep_ace.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template; requires ace includes
#include <JAsynchDataReceiver.h>

class JOutPipeStream;
class JXStaticText;
class CBCmdLineInput;

class CBExecOutputDocument : public CBTextDocument
{
public:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>		RecordLink;
	typedef JAsynchDataReceiver<ACE_LSOCK_STREAM>	DataLink;

public:

	CBExecOutputDocument(const CBTextFileType fileType = kCBExecOutputFT,
						 const JCharacter* helpSectionName = kCBRunProgramHelpName,
						 const JBoolean focusToCmd = kJTrue,
						 const JBoolean allowStop = kJTrue);

	virtual ~CBExecOutputDocument();

	virtual void	Activate();

	void	IncrementUseCount();
	void	DecrementUseCount();

	virtual void	SetConnection(JProcess* p, const int inFD, const int outFD,
								  const JCharacter* windowTitle,
								  const JCharacter* dontCloseMsg,
								  const JCharacter* execDir,
								  const JCharacter* execCmd,
								  const JBoolean showPID);

	JBoolean	ProcessRunning() const;
	void		SendText(const JCharacter* text);

	virtual void	OpenPrevListItem();
	virtual void	OpenNextListItem();

	virtual void	ConvertSelectionToFullPath(JString* fileName) const;

protected:

	JBoolean	ShouldClearWhenStart() const;

	void	ToggleProcessRunning();
	void	StopProcess();
	void	KillProcess();

	virtual void		AppendText(const JString& text);
	virtual JBoolean	ProcessFinished(const JProcess::Finished& info);
	virtual JBoolean	NeedsFormattedData() const;

	JBoolean	GetRecordLink(RecordLink** link) const;
	JBoolean	GetDataLink(DataLink** link) const;

	virtual JBoolean	OKToClose();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	JProcess*		itsProcess;				// deleted when we get a new one
	JString			itsPath;
	JBoolean		itsReceivedDataFlag;
	JBoolean		itsProcessPausedFlag;
	JString			itsDontCloseMsg;
	JBoolean		itsClearWhenStartFlag;
	JSize			itsUseCount;

	RecordLink*		itsRecordLink;			// can be NULL
	DataLink*		itsDataLink;			// can be NULL
	JOutPipeStream*	itsCmdStream;			// can be NULL
	JString			itsLastPrompt;			// latest cmd line prompt (partial message)

	JXTextButton*	itsPauseButton;
	JXTextButton*	itsStopButton;			// can be NULL
	JXTextButton*	itsKillButton;
	JXStaticText*	itsCmdPrompt;
	CBCmdLineInput*	itsCmdInput;
	JXTextButton*	itsEOFButton;
	const JBoolean	itsFocusToCmdFlag;

private:

	void	ReceiveRecord();
	void	ReceiveData(const Message& message);
	void	UpdateButtons();
	void	DeleteLinks();
	void	CloseOutFD();

	// not allowed

	CBExecOutputDocument(const CBExecOutputDocument& source);
	const CBExecOutputDocument& operator=(const CBExecOutputDocument& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFinished;

	class Finished : public JBroadcaster::Message
		{
		public:

			Finished(const JBoolean success, const JBoolean cancelled)
				:
				JBroadcaster::Message(kFinished),
				itsSuccessFlag(JI2B(success && !cancelled)),
				itsCancelledFlag(cancelled),
				itsSomebodyIsWaitingFlag(kJFalse)
				{ };

			JBoolean
			Successful() const
			{
				return itsSuccessFlag;
			};

			JBoolean
			Cancelled() const
			{
				return itsCancelledFlag;
			};

			JBoolean
			SomebodyIsWaiting()
			{
				return itsSomebodyIsWaitingFlag;
			};

			void
			SetSomebodyIsWaiting()
			{
				itsSomebodyIsWaitingFlag = kJTrue;
			};

		private:

			JBoolean	itsSuccessFlag;
			JBoolean	itsCancelledFlag;
			JBoolean	itsSomebodyIsWaitingFlag;
		};
};


/******************************************************************************
 ProcessRunning

 ******************************************************************************/

inline JBoolean
CBExecOutputDocument::ProcessRunning()
	const
{
	return JI2B( itsProcess != NULL && !itsProcess->IsFinished() );
}

/******************************************************************************
 GetRecordLink (protected)

 ******************************************************************************/

inline JBoolean
CBExecOutputDocument::GetRecordLink
	(
	RecordLink** link
	)
	const
{
	*link = itsRecordLink;
	return JI2B( itsRecordLink != NULL );
}

/******************************************************************************
 GetDataLink (protected)

 ******************************************************************************/

inline JBoolean
CBExecOutputDocument::GetDataLink
	(
	DataLink** link
	)
	const
{
	*link = itsDataLink;
	return JI2B( itsDataLink != NULL );
}

/******************************************************************************
 ShouldClearWhenStart (protected)

 ******************************************************************************/

inline JBoolean
CBExecOutputDocument::ShouldClearWhenStart()
	const
{
	return itsClearWhenStartFlag;
}

#endif
