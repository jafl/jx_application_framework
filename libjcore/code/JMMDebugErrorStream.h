/******************************************************************************
 JMMDebugErrorStream.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_JMMDebugErrorStream
#define _H_JMMDebugErrorStream

#include "jx-af/jcore/JMMMonitor.h"

class JMMDebugErrorStream : public JMMMonitor
{
public:

	JMMDebugErrorStream();
	~JMMDebugErrorStream();

protected:

	void HandleObjectDeletedAsArray(const JMMRecord& record) override;
	void HandleArrayDeletedAsObject(const JMMRecord& record) override;

	void HandleUnallocatedDeletion(const JUtf8Byte* file, const JUInt32 line,
								   const bool isArray) override;
	void HandleMultipleDeletion(const JMMRecord& thisRecord,
								const JUtf8Byte* file, const JUInt32 line,
								const bool isArray) override;

	void HandleMultipleAllocation(const JMMRecord& thisRecord,
								  const JMMRecord& firstRecord) override;
};

#endif
