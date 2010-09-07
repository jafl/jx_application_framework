/******************************************************************************
 JMMDebugErrorStream.h

	Copyright © 2010 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JMMDebugErrorStream
#define _H_JMMDebugErrorStream

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JMMMonitor.h>

class JMMDebugErrorStream : public JMMMonitor
{
public:

	JMMDebugErrorStream();
	virtual ~JMMDebugErrorStream();

protected:

	virtual void HandleObjectDeletedAsArray(const JMMRecord& record);
	virtual void HandleArrayDeletedAsObject(const JMMRecord& record);

	virtual void HandleUnallocatedDeletion(const JCharacter* file, const JUInt32 line,
	                                       const JBoolean isArray);
	virtual void HandleMultipleDeletion(const JMMRecord& thisRecord,
	                                    const JCharacter* file, const JUInt32 line,
	                                    const JBoolean isArray);

	void HandleMultipleAllocation(const JMMRecord& thisRecord,
	                              const JMMRecord& firstRecord);

	void HandleNULLDeleted(const JCharacter* file, const JUInt32 line, const JBoolean isArray);

private:

	// not allowed

	JMMDebugErrorStream(const JMMDebugErrorStream& source);
	const JMMDebugErrorStream& operator=(const JMMDebugErrorStream& source);
};

#endif
