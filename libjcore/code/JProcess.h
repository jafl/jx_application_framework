/******************************************************************************
 JProcess.h

	Interface for the JProcess class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JProcess
#define _H_JProcess

#include "JBroadcaster.h"
#include "JPtrArray.h"
#include "jProcessUtil.h"
#include "jSignal.h"
#include <unistd.h>

class JString;

class JProcess : virtual public JBroadcaster
{
public:

	static JError	Create(JProcess** process, const JString& str,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = nullptr,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = nullptr,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = nullptr);

	static JError	Create(JProcess** process,
						   const JPtrArray<JString>& argList,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = nullptr,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = nullptr,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = nullptr);

	static JError	Create(JProcess** process,
						   const JUtf8Byte* argv[], const JSize size,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = nullptr,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = nullptr,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = nullptr);

	static JError	Create(JProcess** process, const JString& workingDirectory,
						   const JString& str,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = nullptr,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = nullptr,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = nullptr);

	static JError	Create(JProcess** process, const JString& workingDirectory,
						   const JPtrArray<JString>& argList,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = nullptr,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = nullptr,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = nullptr);

	static JError	Create(JProcess** process, const JString& workingDirectory,
						   const JUtf8Byte* argv[], const JSize size,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = nullptr,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = nullptr,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = nullptr);

	JProcess(const pid_t pid);

	~JProcess() override;

	pid_t	GetPID() const;
	JError	GetPGID(pid_t* pgid) const;

	void	Quit();
	void	Kill();

	void	WaitUntilFinished();
	bool	IsFinished() const;
	bool	SuccessfulFinish() const;
	bool	GetFinishReason(JChildExitReason* reason, int* result) const;
	bool	GetReturnValue(int* result) const;
	bool	GetTermSignal(int* result) const;
	bool	GetStopSignal(int* result) const;

	JError	SendSignal(const int signal);
	JError	SendSignalToGroup(const int signal);

	JError	SetPriority(const int priority);

	bool	WillDeleteWhenFinished() const;
	void	ShouldDeleteWhenFinished(const bool deleteObj = true);

	static void	CheckForFinishedChild(const bool block);

	bool	WillQuitAtExit() const;
	void	QuitAtExit(const bool quit = true);

	bool	WillKillAtExit() const;
	void	KillAtExit(const bool kill = true);

private:

	const pid_t	itsPID;
	bool		itsIsFinishedFlag;
	int			itsFinishedStatus;
	bool		itsAutoDeleteFlag;	// true => delete when process is finished

	static JPtrArray<JProcess>	theProcessList;		// sorted by pid

private:

	JProcess(const pid_t pid, const int x);

	static std::weak_ordering
		ComparePID(JProcess* const & p1, JProcess* const & p2);

	// not allowed

	JProcess(const JProcess&) = delete;
	JProcess& operator=(const JProcess&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFinished;

	class Finished : public JBroadcaster::Message
		{
		public:

			Finished(const int status)
				:
				JBroadcaster::Message(kFinished),
				itsStatus(status)
				{ };

			bool
			Successful() const
			{
				int result;
				const JChildExitReason reason = JDecodeChildExitReason(itsStatus, &result);
				return reason == kJChildFinished && result == 0;
			};

			JChildExitReason
			GetReason() const
			{
				int result;
				return JDecodeChildExitReason(itsStatus, &result);
			};

			JChildExitReason
			GetReason(int* result) const
			{
				return JDecodeChildExitReason(itsStatus, result);
			};

			bool
			GetReturnValue(int* result) const
			{
				const JChildExitReason reason =
					JDecodeChildExitReason(itsStatus, result);
				return reason == kJChildFinished;
			};

			bool
			GetTermSignal(int* result) const
			{
				const JChildExitReason reason =
					JDecodeChildExitReason(itsStatus, result);
				return reason == kJChildSignalled;
			};

			bool
			GetStopSignal(int* result) const
			{
				const JChildExitReason reason =
					JDecodeChildExitReason(itsStatus, result);
				return reason == kJChildStopped;
			};

		private:

			int	itsStatus;
		};
};


/******************************************************************************
 GetPID

 ******************************************************************************/

inline pid_t
JProcess::GetPID()
	const
{
	return itsPID;
}

/******************************************************************************
 GetPGID

 ******************************************************************************/

inline JError
JProcess::GetPGID
	(
	pid_t* pgid
	)
	const
{
	return JGetPGID(itsPID, pgid);
}

/******************************************************************************
 IsFinished

 ******************************************************************************/

inline bool
JProcess::IsFinished()
	const
{
	return itsIsFinishedFlag;
}

inline bool
JProcess::SuccessfulFinish()
	const
{
	int result;
	const JChildExitReason reason = JDecodeChildExitReason(itsFinishedStatus, &result);
	return itsIsFinishedFlag && reason == kJChildFinished && result == 0;
}

inline bool
JProcess::GetFinishReason
	(
	JChildExitReason*	reason,
	int*				result
	)
	const
{
	*reason = JDecodeChildExitReason(itsFinishedStatus, result);
	return itsIsFinishedFlag;
}

inline bool
JProcess::GetReturnValue
	(
	int* result
	)
	const
{
	const JChildExitReason reason =
		JDecodeChildExitReason(itsFinishedStatus, result);
	return itsIsFinishedFlag && reason == kJChildFinished;
}

inline bool
JProcess::GetTermSignal
	(
	int* result
	)
	const
{
	const JChildExitReason reason =
		JDecodeChildExitReason(itsFinishedStatus, result);
	return itsIsFinishedFlag && reason == kJChildSignalled;
}

inline bool
JProcess::GetStopSignal
	(
	int* result
	)
	const
{
	const JChildExitReason reason =
		JDecodeChildExitReason(itsFinishedStatus, result);
	return itsIsFinishedFlag && reason == kJChildStopped;
}

/******************************************************************************
 SendSignal

 ******************************************************************************/

inline JError
JProcess::SendSignal
	(
	const int signal
	)
{
	return JSendSignalToProcess(itsPID, signal);
}

/******************************************************************************
 Quit

 ******************************************************************************/

inline void
JProcess::Quit()
{
	SendSignalToGroup(SIGTERM);
}

/******************************************************************************
 Kill

 ******************************************************************************/

inline void
JProcess::Kill()
{
	SendSignalToGroup(SIGKILL);
}

/******************************************************************************
 SetPriority

 ******************************************************************************/

inline JError
JProcess::SetPriority
	(
	const int priority
	)
{
	return JSetProcessPriority(itsPID, priority);
}

/******************************************************************************
 Delete when finished

 ******************************************************************************/

inline bool
JProcess::WillDeleteWhenFinished()
	const
{
	return itsAutoDeleteFlag;
}

inline void
JProcess::ShouldDeleteWhenFinished
	(
	const bool deleteObj
	)
{
	itsAutoDeleteFlag = deleteObj;
}

#endif
