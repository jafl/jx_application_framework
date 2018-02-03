/******************************************************************************
 JXRestorePartitionGeometry.cpp

	Ensures that geometry is applied, in case FTC delta is zero.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include <JXRestorePartitionGeometry.h>
#include <JXHorizPartition.h>
#include <JXVertPartition.h>

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
	JXHorizPartition* h = dynamic_cast<JXHorizPartition*>(itsPartition);
	JXVertPartition*  v = dynamic_cast<JXVertPartition*>(itsPartition);

	if (h != NULL && h->itsSavedGeom != NULL)
		{
		h->RestoreGeometry(*h->itsSavedGeom);
		jdelete h->itsSavedGeom;
		h->itsSavedGeom = NULL;
		}
	else if (v != NULL && v->itsSavedGeom != NULL)
		{
		v->RestoreGeometry(*v->itsSavedGeom);
		jdelete v->itsSavedGeom;
		v->itsSavedGeom = NULL;
		}
}
