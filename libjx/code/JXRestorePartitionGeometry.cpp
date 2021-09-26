/******************************************************************************
 JXRestorePartitionGeometry.cpp

	Ensures that geometry is applied, in case FTC delta is zero.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXRestorePartitionGeometry.h"
#include "jx-af/jx/JXHorizPartition.h"
#include "jx-af/jx/JXVertPartition.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRestorePartitionGeometry::JXRestorePartitionGeometry
	(
	JXPartition* partition
	)
	:
	itsPartition(partition)
{
	ClearWhenGoingAway(itsPartition, &itsPartition);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRestorePartitionGeometry::~JXRestorePartitionGeometry()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXRestorePartitionGeometry::Perform()
{
	auto* h = dynamic_cast<JXHorizPartition*>(itsPartition);
	auto*  v = dynamic_cast<JXVertPartition*>(itsPartition);

	if (h != nullptr && h->itsSavedGeom != nullptr)
	{
		h->RestoreGeometry(*h->itsSavedGeom);
		jdelete h->itsSavedGeom;
		h->itsSavedGeom = nullptr;
	}
	else if (v != nullptr && v->itsSavedGeom != nullptr)
	{
		v->RestoreGeometry(*v->itsSavedGeom);
		jdelete v->itsSavedGeom;
		v->itsSavedGeom = nullptr;
	}
}
