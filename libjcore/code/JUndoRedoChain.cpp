/******************************************************************************
 JUndoRedoChain.cpp

	Manages a chain of undo/redo objects and the current location.

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "JUndoRedoChain.h"
#include "JUndo.h"
#include "jAssert.h"

const JSize kDefaultMaxUndoCount = 100;

/******************************************************************************
 Constructor

 ******************************************************************************/

JUndoRedoChain::JUndoRedoChain
	(
	const bool useMultipleUndo
	)
	:
	itsUndo(nullptr),
	itsUndoList(nullptr),
	itsFirstRedoIndex(1),
	itsLastSaveRedoIndex(itsFirstRedoIndex),
	itsUndoState(kIdle),
	itsMaxUndoCount(kDefaultMaxUndoCount)
{
	if (useMultipleUndo)
	{
		itsUndoList = jnew JPtrArray<JUndo>(JPtrArrayT::kDeleteAll, itsMaxUndoCount);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUndoRedoChain::~JUndoRedoChain()
{
	jdelete itsUndo;
	jdelete itsUndoList;
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
JUndoRedoChain::Undo()
{
	assert( itsUndoState == kIdle );

	JUndo* undo;
	if (GetCurrentUndo(&undo))
	{
		itsUndoState = kUndo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
	}
}

/******************************************************************************
 Redo

 ******************************************************************************/

void
JUndoRedoChain::Redo()
{
	assert( itsUndoState == kIdle );

	JUndo* undo;
	if (GetCurrentRedo(&undo))
	{
		itsUndoState = kRedo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
	}
}

/******************************************************************************
 DeactivateCurrentUndo

 ******************************************************************************/

void
JUndoRedoChain::DeactivateCurrentUndo()
{
	JUndo* undo = nullptr;
	if (GetCurrentUndo(&undo))
	{
		undo->Deactivate();
	}
}

/******************************************************************************
 ClearUndo

	Avoid calling this whenever possible since it throws away -all-
	undo information.

 ******************************************************************************/

void
JUndoRedoChain::ClearUndo()
{
	jdelete itsUndo;
	itsUndo = nullptr;

	if (itsUndoList != nullptr)
	{
		itsUndoList->CleanOut();
	}
	itsFirstRedoIndex = 1;
	ClearLastSaveLocation();
}

/******************************************************************************
 SetUndoDepth

 ******************************************************************************/

void
JUndoRedoChain::SetUndoDepth
	(
	const JSize maxUndoCount
	)
{
	assert( maxUndoCount > 0 );

	itsMaxUndoCount = maxUndoCount;
	itsUndoList->SetMinSize(itsMaxUndoCount);
	ClearOutdatedUndo();
}

/******************************************************************************
 GetCurrentUndo

 ******************************************************************************/

bool
JUndoRedoChain::GetCurrentUndo
	(
	JUndo** undo
	)
{
	if (itsUndoList != nullptr && itsFirstRedoIndex > 1)
	{
		*undo = itsUndoList->GetItem(itsFirstRedoIndex - 1);
		return true;
	}
	else if (itsUndoList != nullptr)
	{
		return false;
	}
	else
	{
		*undo = itsUndo;
		return *undo != nullptr;
	}
}

/******************************************************************************
 GetCurrentRedo

 ******************************************************************************/

bool
JUndoRedoChain::GetCurrentRedo
	(
	JUndo** redo
	)
{
	if (itsUndoList != nullptr && itsFirstRedoIndex <= itsUndoList->GetItemCount())
	{
		*redo = itsUndoList->GetItem(itsFirstRedoIndex);
		return true;
	}
	else if (itsUndoList != nullptr)
	{
		return false;
	}
	else
	{
		*redo = itsUndo;
		return *redo != nullptr;
	}
}

/******************************************************************************
 NewUndo

	Register a new Undo object.

	itsFirstRedoIndex points to the first redo object in itsUndoList.
	1 <= itsFirstRedoIndex <= itsUndoList->GetItemCount()+1
	Minimum => everything is redo
	Maximum => everything is undo

 ******************************************************************************/

void
JUndoRedoChain::NewUndo
	(
	JUndo* undo
	)
{
	if (itsUndoList != nullptr && itsUndoState == kIdle)
	{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetItemCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
		{
			itsUndoList->DeleteItem(i);
		}

		if (itsLastSaveRedoIndex > 0 &&
			itsFirstRedoIndex < JIndex(itsLastSaveRedoIndex))
		{
			ClearLastSaveLocation();
		}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		ClearOutdatedUndo();
		assert( !itsUndoList->IsEmpty() );
	}

	else if (itsUndoList != nullptr && itsUndoState == kUndo)
	{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		itsUndoList->SetItem(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(true);
		undo->Deactivate();
	}

	else if (itsUndoList != nullptr && itsUndoState == kRedo)
	{
		assert( itsFirstRedoIndex <= itsUndoList->GetItemCount() );

		itsUndoList->SetItem(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(false);
		undo->Deactivate();
	}

	else
	{
		jdelete itsUndo;
		itsUndo = undo;
	}
}

/******************************************************************************
 ReplaceUndo

 ******************************************************************************/

void
JUndoRedoChain::ReplaceUndo
	(
	JUndo* oldUndo,
	JUndo* newUndo
	)
{
#ifndef NDEBUG

	assert( itsUndoState != kIdle );

	if (itsUndoList != nullptr && itsUndoState == kUndo)
	{
		assert( itsFirstRedoIndex > 1 &&
				oldUndo == itsUndoList->GetItem(itsFirstRedoIndex - 1) );
	}
	else if (itsUndoList != nullptr && itsUndoState == kRedo)
	{
		assert( itsFirstRedoIndex <= itsUndoList->GetItemCount() &&
				oldUndo == itsUndoList->GetItem(itsFirstRedoIndex) );
	}
	else
	{
		assert( oldUndo == itsUndo );
	}

#endif

	NewUndo(newUndo);
}

/******************************************************************************
 ClearOutdatedUndo

 ******************************************************************************/

void
JUndoRedoChain::ClearOutdatedUndo()
{
	if (itsUndoList == nullptr)
	{
		return;
	}

	while (itsUndoList->GetItemCount() > itsMaxUndoCount)
	{
		itsUndoList->DeleteItem(1);
		itsFirstRedoIndex--;
		itsLastSaveRedoIndex--;

		// If we have to throw out redo's, we have to toss everything.

		if (itsFirstRedoIndex == 0)
		{
			ClearUndo();
			break;
		}
	}
}
