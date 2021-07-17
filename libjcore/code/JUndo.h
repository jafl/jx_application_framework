/******************************************************************************
 JUndo.h

	Interface for the JUndo class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUndo
#define _H_JUndo

#include "jTypes.h"

class JUndo
{
public:

	JUndo();

	virtual ~JUndo();

	virtual void	Undo() = 0;

	bool	IsActive() const;
	void		Activate();
	void		Deactivate();

	bool	IsRedo() const;
	void		SetRedo(const bool isRedo);

private:

	bool	itsActiveFlag;
	bool	itsIsRedoFlag;

private:

	// not allowed

	JUndo(const JUndo& source);
	const JUndo& operator=(const JUndo& source);
};


/******************************************************************************
 Active status

 ******************************************************************************/

inline bool
JUndo::IsActive()
	const
{
	return itsActiveFlag;
}

inline void
JUndo::Activate()
{
	itsActiveFlag = true;
}

inline void
JUndo::Deactivate()
{
	itsActiveFlag = false;
}

/******************************************************************************
 Redo status

 ******************************************************************************/

inline bool
JUndo::IsRedo()
	const
{
	return itsIsRedoFlag;
}

inline void
JUndo::SetRedo
	(
	const bool isRedo
	)
{
	itsIsRedoFlag = isRedo;
}

#endif
