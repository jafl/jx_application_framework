/******************************************************************************
 JMMDebugErrorStream.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_JMMDebugErrorStream
#define _H_JMMDebugErrorStream

#include "JMMMonitor.h"

class JMMDebugErrorStream : public JMMMonitor
{
public:

	JMMDebugErrorStream();
	virtual ~JMMDebugErrorStream();

protected:

	virtual void HandleObjectDeletedAsArray(const JMMRecord& record) override;
	virtual void HandleArrayDeletedAsObject(const JMMRecord& record) override;

	virtual void HandleUnallocatedDeletion(const JUtf8Byte* file, const JUInt32 line,
										   const bool isArray) override;
	virtual void HandleMultipleDeletion(const JMMRecord& thisRecord,
										const JUtf8Byte* file, const JUInt32 line,
										const bool isArray) override;

	void HandleMultipleAllocation(const JMMRecord& thisRecord,
								  const JMMRecord& firstRecord) override;
};

#endif
