/******************************************************************************
 JThisProcess.h

	Interface for the JThisProcess class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JThisProcess
#define _H_JThisProcess

#include "JProcess.h"
#include <ace/Event_Handler.h>
#include <ace/Signal.h>
#include <setjmp.h>

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

	// for catching Ctrl-C

	static jmp_buf&	GetSigintJumpBuffer();

protected:

	JThisProcess();

	virtual int	handle_signal(int signum, siginfo_t*, ucontext_t*);

private:

	ACE_Sig_Set	itsSignalSet;
	JBoolean	itsSigintJumpBufferInitFlag;
	jmp_buf		itsSigintJumpBuffer;

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
