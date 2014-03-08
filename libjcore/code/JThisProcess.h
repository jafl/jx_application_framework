/******************************************************************************
 JThisProcess.h

	Interface for the JThisProcess class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JThisProcess
#define _H_JThisProcess

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JProcess.h>
#include <j_prep_ace.h>
#include <ace/Event_Handler.h>
#include <ace/Signal.h>

class JThisProcess : public JProcess, public ACE_Event_Handler
{
public:

	static void				Initialize();
	static JThisProcess*	Instance();

	virtual ~JThisProcess();

	static JError	Fork(pid_t* pid);

	static void	Exit(const int returnValue);
	static void	Abort();

	static JBoolean	WillQuitAtExit(const JProcess* p);
	static void		QuitAtExit(JProcess* p, const JBoolean quit = kJTrue);

	static JBoolean	WillKillAtExit(const JProcess* p);
	static void		KillAtExit(JProcess* p, const JBoolean kill = kJTrue);

	static void		Ignore(JProcess* p);

	static JBoolean	WillCatchSignal(const int sig);
	static void		ShouldCatchSignal(const int sig, const JBoolean catchIt);

	// called by event loop

	static JBoolean	CheckForSignals();

	// called by JExecute() if exec() fails

	static void		ChildExecFailed();

	// for use by networking clean-up code

	static void		CheckACEReactor();

protected:

	JThisProcess();

	virtual int	handle_signal(int signum, siginfo_t*, ucontext_t*);

private:

	ACE_Sig_Set	itsSignalSet;

	static JThisProcess* itsSelf;

	static JPtrArray<JProcess>	theQuitList;
	static JPtrArray<JProcess>	theKillList;

private:

	JBoolean	BroadcastSignal(const int sig);

	static void	CleanUpProcesses();

	// not allowed

	JThisProcess(const JThisProcess& source);
	const JThisProcess& operator=(const JThisProcess& source);

public:

	// JBroadcaster messages -- remember to update kSignalValue, BroadcastSignal()
	// (Check docs for which ones are actually broadcast.)

	static const JCharacter* kIllegalInstruction;
	static const JCharacter* kFPE;
	static const JCharacter* kSegFault;
	static const JCharacter* kBrokenPipe;
	static const JCharacter* kAbort;
	static const JCharacter* kKeyboardInterrupt;

	static const JCharacter* kTerminate;
	static const JCharacter* kKeyboardQuit;

	static const JCharacter* kParentProcessFinished;
	static const JCharacter* kChildProcessFinished;
	static const JCharacter* kTTYInput;
	static const JCharacter* kTTYOutput;

	static const JCharacter* kTimerFinished;
	static const JCharacter* kUserSignal1;
	static const JCharacter* kUserSignal2;

	static const JCharacter* kUnrecognized;

	class Signal : public JBroadcaster::Message
		{
		public:

			Signal(const JCharacter* type, const int value)
				:
				JBroadcaster::Message(type),
				itsSignalValue(value),
				itsCaughtFlag(kJFalse)
				{ };

			int
			GetValue() const
			{
				return itsSignalValue;
			};

			JBoolean
			WasCaught() const
			{
				return itsCaughtFlag;
			};

			void
			SetCaught()
			{
				itsCaughtFlag = kJTrue;
			};

		private:

			int			itsSignalValue;
			JBoolean	itsCaughtFlag;
		};
};


/******************************************************************************
 Initialize (static)

	Call this or Instance() to install the signal handlers.

 ******************************************************************************/

inline void
JThisProcess::Initialize()
{
	Instance();
}

#endif
