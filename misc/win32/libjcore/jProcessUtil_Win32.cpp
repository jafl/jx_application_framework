/******************************************************************************
 jProcessUtil_UNIX.cc

	Routines for creating child processes.

	Copyright (C) 1997-2005 John Lindal.

 ******************************************************************************/

#include <jProcessUtil.h>
#include <jAssert.h>

/******************************************************************************
 JSetProcessPriority

 ******************************************************************************/

JError
JSetProcessPriority
	(
	const pid_t	pid,
	const int	priority
	)
{
	return JUnexpectedError(-1);
}

/******************************************************************************
 JSendSignalToGroup

 ******************************************************************************/

JError
JSendSignalToGroup
	(
	const pid_t	pgid,
	const int	signal
	)
{
	return JUnexpectedError(-1);
}

/******************************************************************************
 JGetPGID

 ******************************************************************************/

JError
JGetPGID
	(
	const pid_t	pid,
	pid_t*		pgid
	)
{
	return JUnexpectedError(-1);
}
