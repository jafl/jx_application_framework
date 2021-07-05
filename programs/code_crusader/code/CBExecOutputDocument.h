/******************************************************************************
 CBExecOutputDocument.h

	Interface for the CBExecOutputDocument class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBExecOutputDocument
#define _H_CBExecOutputDocument

#include "CBTextDocument.h"
#include <JProcess.h>			// need definition of JProcess::Finished
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template; requires ace includes
#include <JAsynchDataReceiver.h>

class JOutPipeStream;
class JXStaticText;
class CBCmdLineInput;

class CBExecOutputDocument : public CBTextDocument
{
	friend class CBExecOutputPostFTCTask;

public:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>		RecordLink;
	typedef JAsynchDataReceiver<ACE_LSOCK_STREAM>	DataLink;

public:

	CBExecOutputDocument(const CBTextFileType fileType = kCBExecOutputFT,
						 const JUtf8Byte* helpSectionName = "CBRunProgramHelp",
						 const JBoolean focusToCmd = kJTrue,
						 const JBoolean allowStop = kJTrue);

	virtual ~CBExecOutputDocument();

	virtual void	Activate() override;

	void	IncrementUseCount();
	void	DecrementUseCount();

	virtual void	SetConnection(JProcess* p, const int inFD, const int outFD,
								  const JString& windowTitle,
								  const JString& dontCloseMsg,
								  const JString& execDir,
								  const JString& execCmd,
								  const JBoolean showPID);

	JBoolean	ProcessRunning() const;
	void		SendText(const JString& text);

	virtual void	OpenPrevListItem();
	virtual void	OpenNextListItem();

	virtual void	ConvertSelectionToFullPath(JString* fileName) const override;

protected:

	JBoolean	ShouldClearWhenStart() const;

	void	ToggleProcessRunning();
	void	StopProcess();
	void	KillProcess();

	virtual void		PlaceCmdLineWidgets();
	virtual void		AppendText(const JString& text);
	virtual JBoolean	ProcessFinished(const JProcess::Finished& info);
	virtual JBoolean	NeedsFormattedData() const;

	JBoolean	GetRecordLink(RecordLink** link) const;
	JBoolean	GetDataLink(DataLink** link) const;

	virtual JBoolean	OKToClose() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	JProcess*		itsProcess;				// deleted when we get a new one
	JString			itsPath;
	JBoolean		itsReceivedDataFlag;
	JBoolean		itsProcessPausedFlag;
	JString			itsDontCloseMsg;
	JBoolean		itsClearWhenStartFlag;
	JSize			itsUseCount;

	RecordLink*		itsRecordLink;			// can be nullptr
	DataLink*		itsDataLink;			// can be nullptr
	JOutPipeStream*	itsCmdStream;			// can be nullptr
	JString			itsLastPrompt;			// latest cmd line prompt (partial message)

	JXTextButton*	itsPauseButton;
	JXTextButton*	itsStopButton;			// can be nullptr
	JXTextButton*	itsKillButton;
	JXStaticText*	itsCmdPrompt;
	CBCmdLineInput*	itsCmdInput;
	JXTextButton*	itsEOFButton;
	const JBoolean	itsFocusToCmdFlag;

private:

	void	ReceiveRecord();
	void	ReceiveData(const Message& message);
	void	UpdateButtons();
	void	CloseOutFD();

	// not allowed

	CBExecOutputDocument(const CBExecOutputDocument& source);
	const CBExecOutputDocument& operator=(const CBExecOutputDocument& source);

	static CBTextEditor*	ConstructTextEditor(CBTextDocument* document,
												const JString& fileName,
												JXMenuBar* menuBar,
												CBTELineIndexInput* lineInput,
												CBTEColIndexInput* colInput,
												JXScrollbarSet* scrollbarSet);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFinished;

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
	return JI2B( itsProcess != nullptr && !itsProcess->IsFinished() );
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
	return JI2B( itsRecordLink != nullptr );
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
	return JI2B( itsDataLink != nullptr );
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
