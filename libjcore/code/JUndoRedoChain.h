/******************************************************************************
 JUndoRedoChain.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUndoRedoChain
#define _H_JUndoRedoChain

#include "JPtrArray.h"

class JUndo;

class JUndoRedoChain
{
public:

	JUndoRedoChain(const bool useMultipleUndo);

	~JUndoRedoChain();

	void	Undo();
	void	Redo();
	void	ClearUndo();
	void	DeactivateCurrentUndo();

	bool	GetUndoList(JPtrArray<JUndo>** list);
	bool	GetCurrentUndo(JUndo** undo);
	bool	GetCurrentRedo(JUndo** redo);
	void	NewUndo(JUndo* undo);
	void	ReplaceUndo(JUndo* oldUndo, JUndo* newUndo);
	void	ClearOutdatedUndo();

	bool	HasSingleUndo() const;
	bool	HasMultipleUndo(bool* canUndo, bool* canRedo) const;

	JSize	GetUndoDepth() const;
	void	SetUndoDepth(const JSize maxUndoCount);

	bool	IsAtLastSaveLocation() const;
	void	SetLastSaveLocation();
	void	ClearLastSaveLocation();

private:

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};

private:

	JUndo*				itsUndo;				// can be nullptr
	JPtrArray<JUndo>*	itsUndoList;			// nullptr if not multiple undo
	JIndex				itsFirstRedoIndex;		// range [1:count+1]
	JInteger			itsLastSaveRedoIndex;	// index where text was saved -- can be outside range of itsUndoList!
	UndoState			itsUndoState;
	JSize				itsMaxUndoCount;		// maximum length of itsUndoList
};


/******************************************************************************
 Has undo

 ******************************************************************************/

inline bool
JUndoRedoChain::HasSingleUndo()
	const
{
	return itsUndo != nullptr;
}

inline bool
JUndoRedoChain::HasMultipleUndo
	(
	bool* canUndo,
	bool* canRedo
	)
	const
{
	*canUndo = itsFirstRedoIndex > 1;
	*canRedo = itsUndoList != nullptr && itsFirstRedoIndex <= itsUndoList->GetItemCount();
	return itsUndoList != nullptr;
}

/******************************************************************************
 GetUndoDepth

 ******************************************************************************/

inline JSize
JUndoRedoChain::GetUndoDepth()
	const
{
	return itsMaxUndoCount;
}

/******************************************************************************
 Last save location

 ******************************************************************************/

inline bool
JUndoRedoChain::IsAtLastSaveLocation()
	const
{
	return (itsUndoList != nullptr &&
			itsLastSaveRedoIndex > 0 &&
			JIndex(itsLastSaveRedoIndex) == itsFirstRedoIndex);
}

inline void
JUndoRedoChain::SetLastSaveLocation()
{
	itsLastSaveRedoIndex = itsFirstRedoIndex;
}

inline void
JUndoRedoChain::ClearLastSaveLocation()
{
	itsLastSaveRedoIndex = 0;
}

/******************************************************************************
 GetUndoList

 ******************************************************************************/

inline bool
JUndoRedoChain::GetUndoList
	(
	JPtrArray<JUndo>** list
	)
{
	*list = itsUndoList;
	return *list != nullptr;
}

#endif
