/******************************************************************************
 JXRestorePartitionGeometry.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRestorePartitionGeometry
#define _H_JXRestorePartitionGeometry

#include "JXUrgentTask.h"

class JXPartition;

class JXRestorePartitionGeometry : public JXUrgentTask
{
public:

	JXRestorePartitionGeometry(JXPartition* partition);

protected:

	~JXRestorePartitionGeometry() override;

	void	Perform() override;

private:

	JXPartition*	itsPartition;	// not owned
};

#endif
