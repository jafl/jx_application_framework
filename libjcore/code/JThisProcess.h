/******************************************************************************
 JThisProcess.h

	Interface for the JThisProcess class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JThisProcess
#define _H_JThisProcess

#include "jx-af/jcore/JProcess.h"
#include <ace/Event_Handler.h>
#include <ace/Signal.h>
#include <setjmp.h>

class JThisProcess : public JProcess, public ACE_Event_Handler
{
public:

	static void				Initialize();
	static JThisProcess*	Instance();

	~JThisProcess();

	static JError	Fork(pid_t* pid);

	[[noreturn]] static void	Exit(const int returnValue);
	[[noreturn]] static void	Abort();

	static bool	WillQuitAtExit(const JProcess* p);
	static void	QuitAtExit(JProcess* p, const bool quit = true);

	static bool	WillKillAtExit(const JProcess* p);
	static void	KillAtExit(JProcess* p, const bool kill = true);

	static void	Ignore(JProcess* p);

	static bool	WillCatchSignal(const int sig);
	static void	ShouldCatchSignal(const int sig, const bool catchIt);

	// called by event loop

	static bool	CheckForSignals();

	// called by JExecute() if exec() fails

	static void	ChildExecFailed();

	// for use by networking clean-up code

	static void	CheckACEReactor();

	// for catching Ctrl-C

	static jmp_buf&	GetSigintJumpBuffer();

protected:

	JThisProcess();

	int	handle_signal(int signum, siginfo_t*, ucontext_t*) override;

private:

	ACE_Sig_Set	itsSignalSet;
	bool		itsSigintJumpBufferInitFlag;
	jmp_buf		itsSigintJumpBuffer;

	static JThisProcess* itsSelf;

	static JPtrArray<JProcess>	theQuitList;
	static JPtrArray<JProcess>	theKillList;

private:

	bool	BroadcastSignal(const int sig);

	static void	CleanUpProcesses();

public:

	// JBroadcaster messages -- remember to update kSignalValue, BroadcastSignal()
	// (Check docs for which ones are actually broadcast.)

	static const JUtf8Byte* kIllegalInstruction;
	static const JUtf8Byte* kFPE;
	static const JUtf8Byte* kSegFault;
	static const JUtf8Byte* kBrokenPipe;
	static const JUtf8Byte* kAbort;
	static const JUtf8Byte* kKeyboardInterrupt;

	static const JUtf8Byte* kTerminate;
	static const JUtf8Byte* kKeyboardQuit;

	static const JUtf8Byte* kParentProcessFinished;
	static const JUtf8Byte* kChildProcessFinished;
	static const JUtf8Byte* kTTYInput;
	static const JUtf8Byte* kTTYOutput;

	static const JUtf8Byte* kTimerFinished;
	static const JUtf8Byte* kUserSignal1;
	static const JUtf8Byte* kUserSignal2;

	static const JUtf8Byte* kUnrecognized;

	class Signal : public JBroadcaster::Message
		{
		public:

			Signal(const JUtf8Byte* type, const int value)
				:
				JBroadcaster::Message(type),
				itsSignalValue(value),
				itsCaughtFlag(false)
				{ };

			int
			GetValue() const
			{
				return itsSignalValue;
			};

			bool
			WasCaught() const
			{
				return itsCaughtFlag;
			};

			void
			SetCaught()
			{
				itsCaughtFlag = true;
			};

		private:

			int			itsSignalValue;
			bool	itsCaughtFlag;
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
