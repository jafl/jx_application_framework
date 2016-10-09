/******************************************************************************
 JProcess.h

	Interface for the JProcess class

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JProcess
#define _H_JProcess

#include <JBroadcaster.h>
#include <JPtrArray.h>
#include <jProcessUtil.h>
#include <jSignal.h>
#include <unistd.h>

class JString;

class JProcess : virtual public JBroadcaster
{
public:

	static JError	Create(JProcess** process, const JCharacter* str,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = NULL,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = NULL,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = NULL);

	static JError	Create(JProcess** process,
						   const JPtrArray<JString>& argList,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = NULL,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = NULL,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = NULL);

	static JError	Create(JProcess** process,
						   const JCharacter* argv[], const JSize size,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = NULL,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = NULL,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = NULL);

	static JError	Create(JProcess** process, const JCharacter* workingDirectory,
						   const JCharacter* str,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = NULL,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = NULL,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = NULL);

	static JError	Create(JProcess** process, const JCharacter* workingDirectory,
						   const JPtrArray<JString>& argList,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = NULL,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = NULL,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = NULL);

	static JError	Create(JProcess** process, const JCharacter* workingDirectory,
						   const JCharacter* argv[], const JSize size,
						   const JExecuteAction toAction = kJIgnoreConnection,
						   int* toFD = NULL,
						   const JExecuteAction fromAction = kJIgnoreConnection,
						   int* fromFD = NULL,
						   const JExecuteAction errAction = kJIgnoreConnection,
						   int* errFD = NULL);

	JProcess(const pid_t pid);

	virtual ~JProcess();

	pid_t	GetPID() const;
	JError	GetPGID(pid_t* pgid) const;

	void	Quit();
	void	Kill();

	void		WaitUntilFinished();
	JBoolean	IsFinished() const;
	JBoolean	SuccessfulFinish() const;
	JBoolean	GetFinishReason(JChildExitReason* reason, int* result) const;
	JBoolean	GetReturnValue(int* result) const;
	JBoolean	GetTermSignal(int* result) const;
	JBoolean	GetStopSignal(int* result) const;

	JError	SendSignal(const int signal);
	JError	SendSignalToGroup(const int signal);

	JError	SetPriority(const int priority);

	JBoolean	WillDeleteWhenFinished() const;
	void		ShouldDeleteWhenFinished(const JBoolean deleteObj = kJTrue);

	static void	CheckForFinishedChild(const JBoolean block);

	JBoolean	WillQuitAtExit() const;
	void		QuitAtExit(const JBoolean quit = kJTrue);

	JBoolean	WillKillAtExit() const;
	void		KillAtExit(const JBoolean kill = kJTrue);

private:

	pid_t		itsPID;
	JBoolean	itsIsFinishedFlag;
	int			itsFinishedStatus;
	JBoolean	itsAutoDeleteFlag;	// kJTrue => jdelete when process is finished

	static JPtrArray<JProcess>	theProcessList;		// sorted by pid

private:

	JProcess(const pid_t pid, const int x);

	static JOrderedSetT::CompareResult
		ComparePID(JProcess* const & p1, JProcess* const & p2);

	// not allowed

	JProcess(const JProcess& source);
	const JProcess& operator=(const JProcess& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFinished;

	class Finished : public JBroadcaster::Message
		{
		public:

			Finished(const int status)
				:
				JBroadcaster::Message(kFinished),
				itsStatus(status)
				{ };

			JBoolean
			Successful() const
			{
				int result;
				const JChildExitReason reason = JDecodeChildExitReason(itsStatus, &result);
				return JConvertToBoolean( reason == kJChildFinished && result == 0 );
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

			JBoolean
			GetReturnValue(int* result) const
			{
				const JChildExitReason reason =
					JDecodeChildExitReason(itsStatus, result);
				return JConvertToBoolean( reason == kJChildFinished );
			};

			JBoolean
			GetTermSignal(int* result) const
			{
				const JChildExitReason reason =
					JDecodeChildExitReason(itsStatus, result);
				return JConvertToBoolean( reason == kJChildSignalled );
			};

			JBoolean
			GetStopSignal(int* result) const
			{
				const JChildExitReason reason =
					JDecodeChildExitReason(itsStatus, result);
				return JConvertToBoolean( reason == kJChildStopped );
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

inline JBoolean
JProcess::IsFinished()
	const
{
	return itsIsFinishedFlag;
}

inline JBoolean
JProcess::SuccessfulFinish()
	const
{
	int result;
	const JChildExitReason reason = JDecodeChildExitReason(itsFinishedStatus, &result);
	return JConvertToBoolean( itsIsFinishedFlag && reason == kJChildFinished && result == 0 );
}

inline JBoolean
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

inline JBoolean
JProcess::GetReturnValue
	(
	int* result
	)
	const
{
	const JChildExitReason reason =
		JDecodeChildExitReason(itsFinishedStatus, result);
	return JConvertToBoolean( itsIsFinishedFlag && reason == kJChildFinished );
}

inline JBoolean
JProcess::GetTermSignal
	(
	int* result
	)
	const
{
	const JChildExitReason reason =
		JDecodeChildExitReason(itsFinishedStatus, result);
	return JConvertToBoolean( itsIsFinishedFlag && reason == kJChildSignalled );
}

inline JBoolean
JProcess::GetStopSignal
	(
	int* result
	)
	const
{
	const JChildExitReason reason =
		JDecodeChildExitReason(itsFinishedStatus, result);
	return JConvertToBoolean( itsIsFinishedFlag && reason == kJChildStopped );
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

inline JBoolean
JProcess::WillDeleteWhenFinished()
	const
{
	return itsAutoDeleteFlag;
}

inline void
JProcess::ShouldDeleteWhenFinished
	(
	const JBoolean deleteObj
	)
{
	itsAutoDeleteFlag = deleteObj;
}

#endif
