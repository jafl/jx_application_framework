/******************************************************************************
 CBCommand.h

	Copyright (C) 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCommand
#define _H_CBCommand

#include "CBCommandManager.h"	// need defn of CmdInfo
#include <JPtrArray-JString.h>

class JProcess;
class CBProjectDocument;
class CBExecOutputDocument;

class CBCommand : virtual public JBroadcaster
{
public:

	CBCommand(const JCharacter* path, const JBoolean refreshCVSStatusWhenFinished,
			  const JBoolean beepWhenFinished, CBProjectDocument* projDoc);

	virtual	~CBCommand();

	void		SetParent(CBCommand* cmd);
	JBoolean	Add(const JCharacter* origCmd, CBTextDocument* textDoc,
					const JPtrArray<JString>* fullNameList,
					const JArray<JIndex>* lineIndexList,
					JPtrArray<JString>* fnList);
	void		Add(CBCommand* subCmd, const CBCommandManager::CmdInfo& cmdInfo);
	void		MarkEndOfSequence();
	JBoolean	Start(const CBCommandManager::CmdInfo& info);
	JBoolean	StartMakeProcess(CBExecOutputDocument* outputDoc);

	const JString&	GetPath() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

private:

	struct CmdInfo
	{
		JString*					cmd;		// can be NULL
		CBCommand*					cmdObj;		// can be NULL
		CBCommandManager::CmdInfo*	cmdInfo;	// NULL if cmdObj==NULL
		JBoolean					isMakeDepend;

		CmdInfo()
			:
			cmd(NULL), cmdObj(NULL), cmdInfo(NULL), isMakeDepend(kJFalse)
		{ };

		CmdInfo(JString* c, CBCommand* o, CBCommandManager::CmdInfo* i,
				const JBoolean isMD)
			:
			cmd(c), cmdObj(o), cmdInfo(i), isMakeDepend(isMD)
		{ };

		JBoolean	IsEndOfSequence() const;
		JBoolean	IsSubroutine() const;
		void		Free(const JBoolean deleteCmdObj);
	};

private:

	CBProjectDocument*		itsProjDoc;			// can be NULL
	const JString			itsCmdPath;
	JArray<CmdInfo>*		itsCmdList;
	CBExecOutputDocument*	itsOutputDoc;		// not owned; can be NULL
	JString					itsWindowTitle;
	JString					itsDontCloseMsg;
	const JBoolean			itsBeepFlag;
	JBoolean				itsRefreshCVSStatusFlag;
	JBoolean				itsUpdateSymbolDatabaseFlag;
	JBoolean				itsInQueueFlag;
	JBoolean				itsSuccessFlag;
	JBoolean				itsCancelledFlag;
	CBCommand*				itsMakeDependCmd;

	// used for subroutines

	CBExecOutputDocument*	itsBuildOutputDoc;	// can be NULL; ensures single window for subroutines
	CBExecOutputDocument*	itsRunOutputDoc;	// can be NULL; ensures single window for subroutines
	CBCommand*				itsParent;			// can be NULL; not owned; parent who Start()ed us
	JBoolean				itsCallParentProcessFinishedFlag;

private:

	JBoolean	StartProcess();
	void		ProcessFinished(const JBoolean success, const JBoolean cancelled);

	void	SetCompileDocStrings();

	void	DeleteThis();
	void	FinishWindow(CBExecOutputDocument** doc);

	void	ReportInfiniteLoop(const JPtrArray<JString>& cmdList,
							   const JIndex startIndex);

	// not allowed

	CBCommand(const CBCommand& source);
	const CBCommand& operator=(const CBCommand& source);

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
				itsCancelledFlag(cancelled)
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

		private:

			const JBoolean	itsSuccessFlag;
			const JBoolean	itsCancelledFlag;
		};
};


/******************************************************************************
 GetPath

 ******************************************************************************/

inline const JString&
CBCommand::GetPath()
	const
{
	return itsCmdPath;
}

/******************************************************************************
 SetParent

 ******************************************************************************/

inline void
CBCommand::SetParent
	(
	CBCommand* cmd
	)
{
	itsParent = cmd;
}

#endif
