/******************************************************************************
 JXRestorePartitionGeometry.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRestorePartitionGeometry
#define _H_JXRestorePartitionGeometry

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXPartition;

class JXRestorePartitionGeometry : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXRestorePartitionGeometry(JXPartition* partition);

	virtual ~JXRestorePartitionGeometry();

	virtual void	Perform();

private:

	JXPartition*	itsPartition;	// not owned

private:

	// not allowed

	JXRestorePartitionGeometry(const JXRestorePartitionGeometry& source);
	const JXRestorePartitionGeometry& operator=(const JXRestorePartitionGeometry& source);
};

#endif
