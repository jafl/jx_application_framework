/******************************************************************************
 jSignal.h

	Augments signal.h

	Copyright © 1995-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jSignal
#define _H_jSignal

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <signal.h>
#include <jMissingProto.h>

#ifdef _MSC_VER
#define SIGHUP  0
#define SIGQUIT 0
#define SIGKILL 0
#define SIGPIPE	0
#define SIGALRM	0
#define SIGSTOP	0
#define SIGCONT 0
#define SIGBUS  0
#define SIGUSR1	0
#define SIGUSR2	0
#define SIGCHLD	0
#define SIGTSTP	0
#define SIGTTIN	0
#define SIGTTOU	0
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (j_sig_func)(int);

#ifdef __cplusplus
}
#endif

class JString;

JString JGetSignalName(const int value);

#endif
