/******************************************************************************
 JThisProcess.cpp

	Class to represent one's own UNIX process and catch signals.  This class
	is designed to be a single, global object, accessible via
	JThisProcess::Instance().

	Event loops can call CheckForSignals() in order to convert signals into
	JBroadcaster messages.  The messages are broadcast in the order that
	the signals were received and are caught via ReceiveWithFeedback() so
	that the one who handles the signal can indicate this by calling
	SetCaught() before returning.

	Only the first 32 signals that are received will actually be broadcast.
	This shouldn't be a problem unless the event loop is blocked and the
	program relies heavily on signals, neither of which should happen in a
	well-designed program, I think.  Let me know if you need more.

	You can change which signals are caught by calling ShouldCatchSignal().

	The signals that are caught by default are:

		Signal  Action  Description
		------------------------------------------------------
		SIGPIPE			Broken pipe: write to pipe with no readers
						(non-fatal because no control over when other end dies)

		SIGTERM		Q	Termination signal
		SIGQUIT		Q	Quit from keyboard

		SIGINT		D	Interrupt from keyboard

		SIGHUP			Hangup detected on controlling terminal
						or death of controlling process
		SIGCHLD			Child stopped or terminated
		SIGTTIN			tty input for background process
		SIGTTOU			tty output for background process

		SIGALRM			Timer signal from alarm(1)
		SIGUSR1			User-defined signal 1
		SIGUSR2			User-defined signal 2

		A = Abort if the signal is not caught.
		Q = Cause CheckForSignals() to return true,
			as a suggestion to the event loop to quit,
			if the signal is not caught.
		D = Clean up child processes that should quit or be killed
			and then die

	The signals that are not caught by default are:

		Signal    Description                 Reason
		------------------------------------------------------
		SIGSEGV   Invalid memory reference    How does one recover?
		SIGFPE    Floating point exception    How does one recover?
		SIGILL    Illegal Instruction         How does one recover?

	The signals that should never be caught are:

		Signal    Description                Reason
		------------------------------------------------------
		SIGKILL   Kill signal                Can't be caught
		SIGCONT   Continue if stopped        It does the right thing already
		SIGSTOP   Stop process               Can't be caught
		SIGTSTP   Stop typed at tty          Catching this means Ctrl-Z won't work
		SIGABRT   Abort signal from abort(3) assert() calls abort()
											 abort() generates infinite # of SIGABRT's

	We provide GetSigintJumpBuffer() so SIGINT handler can longjmp() back
	to JXApplication(), call CleanUpBeforeSuddenDeath(), and then exit().

	BASE CLASS = JProcess, ACE_Event_Handler

	Copyright (C) 1997 by John Lindal.

 *****************************************************************************/

#include "jx-af/jcore/JThisProcess.h"
#include <ace/Reactor.h>
#include "jx-af/jcore/JString.h"
#include "jx-af/jcore/JMinMax.h"
#include "jx-af/jcore/JStdError.h"
#include <stdlib.h>
#include "jx-af/jcore/jErrno.h"
#include "jx-af/jcore/jAssert.h"

JThisProcess* JThisProcess::itsSelf = nullptr;

JPtrArray<JProcess> JThisProcess::theQuitList(JPtrArrayT::kForgetAll);
JPtrArray<JProcess> JThisProcess::theKillList(JPtrArrayT::kForgetAll);

// JBroadcaster message types

const JUtf8Byte* JThisProcess::kIllegalInstruction    = "IllegalInstruction::JThisProcess";
const JUtf8Byte* JThisProcess::kFPE                   = "FPE::JThisProcess";
const JUtf8Byte* JThisProcess::kSegFault              = "SegFault::JThisProcess";
const JUtf8Byte* JThisProcess::kBrokenPipe            = "BrokenPipe::JThisProcess";
const JUtf8Byte* JThisProcess::kAbort                 = "Abort::JThisProcess";
const JUtf8Byte* JThisProcess::kKeyboardInterrupt     = "KeyboardInterrupt::JThisProcess";

const JUtf8Byte* JThisProcess::kTerminate             = "Terminate::JThisProcess";
const JUtf8Byte* JThisProcess::kKeyboardQuit          = "KeyboardQuit::JThisProcess";

const JUtf8Byte* JThisProcess::kParentProcessFinished = "ParentProcessFinished::JThisProcess";
const JUtf8Byte* JThisProcess::kChildProcessFinished  = "ChildProcessFinished::JThisProcess";
const JUtf8Byte* JThisProcess::kTTYInput              = "TTYInput::JThisProcess";
const JUtf8Byte* JThisProcess::kTTYOutput             = "TTYOutput::JThisProcess";

const JUtf8Byte* JThisProcess::kTimerFinished         = "TimerFinished::JThisProcess";
const JUtf8Byte* JThisProcess::kUserSignal1           = "UserSignal1::JThisProcess";
const JUtf8Byte* JThisProcess::kUserSignal2           = "UserSignal2::JThisProcess";

const JUtf8Byte* JThisProcess::kUnrecognized          = "Unrecognized::JThisProcess";

// static data

static const int kSignalValue[] =
{
	SIGPIPE,
	SIGTERM, SIGQUIT, SIGINT,
	SIGHUP, SIGCHLD,
	SIGTTIN, SIGTTOU,
	SIGALRM, SIGUSR1, SIGUSR2
};

// Interrupt routine

const JSize kSignalListSize = 32;

volatile std::atomic_int pendingSignalCount = 0;

volatile sig_atomic_t signalList [ kSignalListSize ];

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

JThisProcess*
JThisProcess::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew JThisProcess;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JThisProcess::JThisProcess()
	:
	JProcess(getpid()),
	ACE_Event_Handler(),
	itsSigintJumpBufferInitFlag(false)
{
	QuitAtExit(this, false);	// We don't need to kill ourselves!

	pendingSignalCount = 0;

	// install handlers

	for (auto s : kSignalValue)
	{
		itsSignalSet.sig_add(s);
	}

	(ACE_Reactor::instance())->register_handler(itsSignalSet, this);

	// remember to clean up

	atexit(JThisProcess::CleanUpProcesses);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JThisProcess::~JThisProcess()
{
	itsSelf = nullptr;
}

/******************************************************************************
 BroadcastSignal

 ******************************************************************************/

bool
JThisProcess::BroadcastSignal
	(
	const int sig
	)
{
	const JUtf8Byte* signalType = nullptr;
	if (sig == SIGILL)
	{
		signalType = kIllegalInstruction;
	}
	else if (sig == SIGFPE)
	{
		signalType = kFPE;
	}
	else if (sig == SIGSEGV)
	{
		signalType = kSegFault;
	}
	else if (sig == SIGPIPE)
	{
		signalType = kBrokenPipe;
	}
	else if (sig == SIGABRT)
	{
		signalType = kAbort;
	}
	else if (sig == SIGINT)
	{
		signalType = kKeyboardInterrupt;
	}

	else if (sig == SIGTERM)
	{
		signalType = kTerminate;
	}
	else if (sig == SIGQUIT)
	{
		signalType = kKeyboardQuit;
	}

	else if (sig == SIGHUP)
	{
		signalType = kParentProcessFinished;
	}
	else if (sig == SIGCHLD)
	{
		signalType = kChildProcessFinished;
	}
	else if (sig == SIGTTIN)
	{
		signalType = kTTYInput;
	}
	else if (sig == SIGTTOU)
	{
		signalType = kTTYOutput;
	}

	else if (sig == SIGALRM)
	{
		signalType = kTimerFinished;
	}
	else if (sig == SIGUSR1)
	{
		signalType = kUserSignal1;
	}
	else if (sig == SIGUSR2)
	{
		signalType = kUserSignal2;
	}

	else
	{
		signalType = kUnrecognized;
	}

	Signal s(signalType, sig);
	BroadcastWithFeedback(&s);
	return s.WasCaught();
}

/******************************************************************************
 Catching signals (static)

 ******************************************************************************/

bool
JThisProcess::WillCatchSignal
	(
	const int sig
	)
{
	JThisProcess* p = JThisProcess::Instance();
	return (p->itsSignalSet).is_member(sig);
}

void
JThisProcess::ShouldCatchSignal
	(
	const int		sig,
	const bool	catchIt
	)
{
	JThisProcess* p = JThisProcess::Instance();

	if (catchIt)
	{
		(p->itsSignalSet).sig_add(sig);
		(ACE_Reactor::instance())->register_handler(sig, p);
	}
	else
	{
		(p->itsSignalSet).sig_del(sig);
		(ACE_Reactor::instance())->remove_handler(sig, (ACE_Sig_Action*) nullptr);
	}
}

/******************************************************************************
 CheckForSignals (static)

	Checks if we have received any signals.  If so, it calls
	BroadcastWithFeedback().  Returns true if a signal was received that
	implies a request to quit.

 ******************************************************************************/

bool
JThisProcess::CheckForSignals()
{
	bool requestQuit = false;

	if (pendingSignalCount > 0 && itsSelf != nullptr)
	{
		for (JUnsignedOffset i=0; i < (JSize) pendingSignalCount; i++)
		{
			// this is safe since extra signals are simply appended to the list

			const bool sigCaught = itsSelf->BroadcastSignal(signalList[i]);
			if (!sigCaught)
			{
				if (signalList[i] == SIGTERM ||
					signalList[i] == SIGQUIT)
				{
					requestQuit = true;
				}
				else if (signalList[i] == SIGILL  ||
						 signalList[i] == SIGFPE  ||
						 signalList[i] == SIGSEGV ||
						 signalList[i] == SIGABRT)
				{
					const JString sigName = JGetSignalName(signalList[i]);
					std::cerr << "An unexpected signal (" << sigName << ") was received!" << std::endl;
					assert_msg( 0, "unexpected signal" );
				}
				else if (signalList[i] == SIGPIPE)
				{
					const JString sigName = JGetSignalName(signalList[i]);
					std::cerr << "Non-fatal error:  signal (" << sigName << ") was received" << std::endl;
				}
//				else
//					{
//					const JString sigName = JGetSignalName(signalList[i]);
//					std::cerr << "Received signal " << sigName << std::endl;
//					}
			}
		}

		pendingSignalCount = 0;
	}

	return requestQuit;
}

/******************************************************************************
 handle_signal (virtual protected)

	Sets the appropriate flags so we remember to broadcast next time
	CheckForSignals() is called.

 ******************************************************************************/

int
JThisProcess::handle_signal
	(
	int signum,
	siginfo_t*,
	ucontext_t*
	)
{
	if (signum == SIGINT && itsSigintJumpBufferInitFlag)
	{
		longjmp(itsSigintJumpBuffer, 1);
		// not reached
	}
	else if (signum == SIGINT)
	{
		JThisProcess::Exit(1);
	}
	else if (((JSize) pendingSignalCount) < kSignalListSize)
	{
		signalList[ pendingSignalCount ] = signum;
		pendingSignalCount++;
	}

	return 0;
}

/******************************************************************************
 WillQuitAtExit (static)

	Returns true if the given process will be terminated when we quit.

 ******************************************************************************/

bool
JThisProcess::WillQuitAtExit
	(
	const JProcess* p
	)
{
	return theQuitList.Includes(p);
}

/******************************************************************************
 QuitAtExit (static)

	Specify whether or not the given process should be terminated when we quit.

	The default is for processes to not be terminated.

 ******************************************************************************/

void
JThisProcess::QuitAtExit
	(
	JProcess*		p,
	const bool	quit
	)
{
	theKillList.Remove(p);

	if (quit && p != Instance() && !theQuitList.Includes(p))
	{
		theQuitList.Append(p);
	}
	else if (!quit)
	{
		theQuitList.Remove(p);
	}
}

/******************************************************************************
 WillKillAtExit (static)

	Returns true if the given process will be killed when we quit.

 ******************************************************************************/

bool
JThisProcess::WillKillAtExit
	(
	const JProcess* p
	)
{
	return theKillList.Includes(p);
}

/******************************************************************************
 KillAtExit (static)

	Specify whether or not the given process should be killed when we quit.

	The default is for processes to not be killed.

 ******************************************************************************/

void
JThisProcess::KillAtExit
	(
	JProcess*		p,
	const bool	kill
	)
{
	theQuitList.Remove(p);

	if (kill && p != Instance() && !theKillList.Includes(p))
	{
		theKillList.Append(p);
	}
	else if (!kill)
	{
		theKillList.Remove(p);
	}
}

/******************************************************************************
 Ignore (static)

	Turns off Quit and Kill at exit.

 ******************************************************************************/

void
JThisProcess::Ignore
	(
	JProcess* p
	)
{
	QuitAtExit(p, false);
	KillAtExit(p, false);
}

/******************************************************************************
 Fork (static)

	fork() preserves everything in the child, including child processes
	and JProcess objects.  The child must therefore toss the quit and kill
	lists so other child processes of the parent are not wiped out.

	If no error occurs, *pid is zero inside the child process.

 ******************************************************************************/

JError
JThisProcess::Fork
	(
	pid_t* pid
	)
{
	const int result = ACE_OS::fork();
	if (result == -1)
	{
		*pid = 0;

		const int err = jerrno();
		if (err == EAGAIN)
		{
			return JNoProcessMemory();
		}
		else if (err == ENOMEM)
		{
			return JNoKernelMemory();
		}
		else
		{
			return JUnexpectedError(err);
		}
	}

	*pid = result;
	if (*pid == 0)		// child
	{
		itsSelf = nullptr;
		Initialize();

		ACE_OS::setpgid(0, 0);	// sets our process group id to our process id -- required by JProcess

		theQuitList.RemoveAll();
		theKillList.RemoveAll();
	}

	return JNoError();
}

/******************************************************************************
 Exit (static)

	Cleans up processes and calls exit().

 ******************************************************************************/

void
JThisProcess::Exit
	(
	const int returnValue
	)
{
	CleanUpProcesses();
	exit(returnValue);
}

/******************************************************************************
 Abort (static)

	Cleans up processes and calls abort().

 ******************************************************************************/

void
JThisProcess::Abort()
{
	CleanUpProcesses();
	abort();
}

/******************************************************************************
 CleanUpProcesses (static private)

 ******************************************************************************/

void
JThisProcess::CleanUpProcesses()
{
	for (auto* p : theQuitList)
	{
		p->Quit();
	}
	theQuitList.RemoveAll();

	for (auto* p : theKillList)
	{
		p->Kill();
	}
	theKillList.RemoveAll();
}

/******************************************************************************
 CheckACEReactor (static)

	Bumps the ACE reactor to check sockets, signals, etc.

	It is not generally a good idea to call this if there is a blocking
	window open because it may invoke arbitrary amounts of code, which is
	dangerous since JX is not re-entrant.

 ******************************************************************************/

void
JThisProcess::CheckACEReactor()
{
	ACE_Time_Value timeout(0);
//	ACE_Reactor::run_event_loop(timeout);	// locks up when child process exits

	(ACE_Reactor::instance())->handle_events(timeout);
	ACE_Reactor::check_reconfiguration(nullptr);
}

/******************************************************************************
 GetSigintJumpBuffer (static)

 ******************************************************************************/

jmp_buf&
JThisProcess::GetSigintJumpBuffer()
{
	JThisProcess* p = JThisProcess::Instance();

	p->itsSigintJumpBufferInitFlag = true;
	return p->itsSigintJumpBuffer;
}
