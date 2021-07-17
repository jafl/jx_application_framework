/******************************************************************************
 CBCommand.h

	Copyright © 2002 by John Lindal.

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

	CBCommand(const JString& path, const bool refreshVCSStatusWhenFinished,
			  const bool beepWhenFinished, CBProjectDocument* projDoc);

	virtual	~CBCommand();

	void		SetParent(CBCommand* cmd);
	bool	Add(const JPtrArray<JString>& cmdArgs,
					const JPtrArray<JString>& fullNameList,
					const JArray<JIndex>& lineIndexList,
					CBFunctionStack* fnStack);
	void		Add(CBCommand* subCmd, const CBCommandManager::CmdInfo& cmdInfo);
	void		MarkEndOfSequence();
	bool	Start(const CBCommandManager::CmdInfo& info);
	bool	StartMakeProcess(CBExecOutputDocument* outputDoc);

	const JString&	GetPath() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	struct CmdInfo
	{
		JPtrArray<JString>*			cmd;		// can be nullptr
		CBCommand*					cmdObj;		// can be nullptr
		CBCommandManager::CmdInfo*	cmdInfo;	// nullptr if cmdObj==nullptr
		bool					isMakeDepend;

		CmdInfo()
			:
			cmd(nullptr), cmdObj(nullptr), cmdInfo(nullptr), isMakeDepend(false)
		{ };

		CmdInfo(JPtrArray<JString>* c, CBCommand* o, CBCommandManager::CmdInfo* i,
				const bool isMD)
			:
			cmd(c), cmdObj(o), cmdInfo(i), isMakeDepend(isMD)
		{ };

		bool	IsEndOfSequence() const;
		bool	IsSubroutine() const;
		void		Free(const bool deleteCmdObj);
	};

private:

	CBProjectDocument*		itsProjDoc;			// can be nullptr
	const JString			itsCmdPath;
	JArray<CmdInfo>*		itsCmdList;
	CBExecOutputDocument*	itsOutputDoc;		// not owned; can be nullptr
	JString					itsWindowTitle;
	JString					itsDontCloseMsg;
	const bool			itsBeepFlag;
	bool				itsRefreshVCSStatusFlag;
	bool				itsUpdateSymbolDatabaseFlag;
	bool				itsInQueueFlag;
	bool				itsSuccessFlag;
	bool				itsCancelledFlag;
	CBCommand*				itsMakeDependCmd;

	// used for subroutines

	CBExecOutputDocument*	itsBuildOutputDoc;	// can be nullptr; ensures single window for subroutines
	CBExecOutputDocument*	itsRunOutputDoc;	// can be nullptr; ensures single window for subroutines
	CBCommand*				itsParent;			// can be nullptr; not owned; parent who Start()ed us
	bool				itsCallParentProcessFinishedFlag;

private:

	bool	StartProcess();
	void		ProcessFinished(const bool success, const bool cancelled);

	void	SetCompileDocStrings();

	void	DeleteThis();
	void	FinishWindow(CBExecOutputDocument** doc);

	void	ReportInfiniteLoop(const CBFunctionStack& fnStack,
							   const JIndex startIndex);

	// not allowed

	CBCommand(const CBCommand& source);
	const CBCommand& operator=(const CBCommand& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFinished;

	class Finished : public JBroadcaster::Message
		{
		public:

			Finished(const bool success, const bool cancelled)
				:
				JBroadcaster::Message(kFinished),
				itsSuccessFlag(success && !cancelled),
				itsCancelledFlag(cancelled)
				{ };

			bool
			Successful() const
			{
				return itsSuccessFlag;
			};

			bool
			Cancelled() const
			{
				return itsCancelledFlag;
			};

		private:

			const bool	itsSuccessFlag;
			const bool	itsCancelledFlag;
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
