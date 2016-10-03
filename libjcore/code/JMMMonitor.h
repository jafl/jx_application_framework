/******************************************************************************
 JMMMonitor.h

	Interface for the JMMMonitor class.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JMMMonitor
#define _H_JMMMonitor

// Superclass Header
#include <JBroadcaster.h>

#include <jTypes.h>

	class JMMRecord;


class JMMMonitor : virtual public JBroadcaster
{
public:

	virtual ~JMMMonitor();

protected:

	// No darn reason to allow this class to be instantiated.
	JMMMonitor();

	virtual void Receive(JBroadcaster* sender, const Message& message);

	// JMemoryManager message handlers; all do nothing by default

	virtual void HandleObjectDeletedAsArray(const JMMRecord& record);
	virtual void HandleArrayDeletedAsObject(const JMMRecord& record);

	virtual void HandleUnallocatedDeletion(const JCharacter* file, const JUInt32 line,
	                                       const JBoolean isArray);
	virtual void HandleMultipleDeletion(const JMMRecord& thisRecord,
	                                    const JCharacter* file, const JUInt32 line,
	                                    const JBoolean isArray);

	virtual void HandleMultipleAllocation(const JMMRecord& thisRecord,
	                                      const JMMRecord& firstRecord);

	virtual void HandleNULLDeleted(const JCharacter* file, const JUInt32 line,
	                               const JBoolean isArray);

private:

	// not allowed

	JMMMonitor(const JMMMonitor& source);
	const JMMMonitor& operator=(const JMMMonitor& source);
};

#endif
