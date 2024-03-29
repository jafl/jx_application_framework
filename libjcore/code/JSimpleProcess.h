/******************************************************************************
 JSimpleProcess.h

	Interface for the JSimpleProcess class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JSimpleProcess
#define _H_JSimpleProcess

#include "JProcess.h"
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include "JMessageProtocol.h"	// template; requires ace includes

class JSimpleProcess : public JProcess
{
public:

	static JError	Create(const JString& str, const bool detach);
	static JError	Create(const JPtrArray<JString>& argList,
						   const bool detach);
	static JError	Create(const JUtf8Byte* argv[], const JSize size,
						   const bool detach);

	static JError	Create(const JString& workingDirectory,
						   const JString& str, const bool detach);
	static JError	Create(const JString& workingDirectory,
						   const JPtrArray<JString>& argList, const bool detach);
	static JError	Create(const JString& workingDirectory,
						   const JUtf8Byte* argv[], const JSize size,
						   const bool detach);

	static JError	Create(JSimpleProcess** process, const JString& str,
						   const bool deleteWhenFinished = false);
	static JError	Create(JSimpleProcess** process, const JString& workingDirectory,
						   const JString& str,
						   const bool deleteWhenFinished = false);

	static JError	Create(JSimpleProcess** process,
						   const JPtrArray<JString>& argList,
						   const bool deleteWhenFinished = false);
	static JError	Create(JSimpleProcess** process, const JString& workingDirectory,
						   const JPtrArray<JString>& argList,
						   const bool deleteWhenFinished = false);

	static JError	Create(JSimpleProcess** process,
						   const JUtf8Byte* argv[], const JSize size,
						   const bool deleteWhenFinished = false);
	static JError	Create(JSimpleProcess** process, const JString& workingDirectory,
						   const JUtf8Byte* argv[], const JSize size,
						   const bool deleteWhenFinished = false);

	JSimpleProcess(const pid_t pid, const int fd,
				   const bool deleteWhenFinished);

	~JSimpleProcess() override;

	void	SetMaxReportInterval(const time_t delta);
	void	ReportError(const bool success);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	using ProcessLink = JMessageProtocol<ACE_LSOCK_STREAM>;

private:

	ProcessLink*	itsLink;				// keeps pipe from filling up and blocking
	const time_t	itsStartTime;			// time when process was created
	time_t			itsMaxReportInterval;	// seconds
};


/******************************************************************************
 SetMaxReportInterval

 ******************************************************************************/

inline void
JSimpleProcess::SetMaxReportInterval
	(
	const time_t delta
	)
{
	itsMaxReportInterval = delta;
}

#endif
