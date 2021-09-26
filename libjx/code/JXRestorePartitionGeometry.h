/******************************************************************************
 JXRestorePartitionGeometry.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRestorePartitionGeometry
#define _H_JXRestorePartitionGeometry

#include "jx-af/jx/JXUrgentTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXPartition;

class JXRestorePartitionGeometry : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXRestorePartitionGeometry(JXPartition* partition);

	virtual ~JXRestorePartitionGeometry();

	virtual void	Perform();

private:

	JXPartition*	itsPartition;	// not owned
};

#endif
