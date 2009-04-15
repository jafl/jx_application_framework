/******************************************************************************
 JUndo.cpp

	Base class to support the fundamental concept of undoing an operation.
	We require that all derived classes implement Undo().

	We maintain an "active" flag because some derived classes will accumulate
	a set of changes into one undo action.  A deactivated Undo object can no
	longer accumulate changes and must therefore be replaced before the next
	change.

	We also maintain a "redo" flag so other objects can tell whether we
	are actually performing an undo or a redo operation.  (Redo objects should
	only be created by Undo objects.)

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JUndo.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JUndo::JUndo()
{
	itsActiveFlag = kJTrue;
	itsIsRedoFlag = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUndo::~JUndo()
{
}
