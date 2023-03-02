/******************************************************************************
 JXRestorePartitionGeometry.cpp

	Ensures that geometry is applied, in case FTC delta is zero.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "JXRestorePartitionGeometry.h"
#include "JXHorizPartition.h"
#include "JXVertPartition.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRestorePartitionGeometry::JXRestorePartitionGeometry
	(
	JXPartition* partition
	)
	:
	JXUrgentTask(partition),
	itsPartition(partition)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXRestorePartitionGeometry::~JXRestorePartitionGeometry()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXRestorePartitionGeometry::Perform()
{
	auto* h = dynamic_cast<JXHorizPartition*>(itsPartition);
	auto* v = dynamic_cast<JXVertPartition*>(itsPartition);

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
