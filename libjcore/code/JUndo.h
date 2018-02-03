/******************************************************************************
 JUndo.h

	Interface for the JUndo class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUndo
#define _H_JUndo

#include <jTypes.h>

class JUndo
{
public:

	JUndo();

	virtual ~JUndo();

	virtual void	Undo() = 0;

	JBoolean	IsActive() const;
	void		Activate();
	void		Deactivate();

	JBoolean	IsRedo() const;
	void		SetRedo(const JBoolean isRedo);

private:

	JBoolean	itsActiveFlag;
	JBoolean	itsIsRedoFlag;

private:

	// not allowed

	JUndo(const JUndo& source);
	const JUndo& operator=(const JUndo& source);
};


/******************************************************************************
 Active status

 ******************************************************************************/

inline JBoolean
JUndo::IsActive()
	const
{
	return itsActiveFlag;
}

inline void
JUndo::Activate()
{
	itsActiveFlag = kJTrue;
}

inline void
JUndo::Deactivate()
{
	itsActiveFlag = kJFalse;
}

/******************************************************************************
 Redo status

 ******************************************************************************/

inline JBoolean
JUndo::IsRedo()
	const
{
	return itsIsRedoFlag;
}

inline void
JUndo::SetRedo
	(
	const JBoolean isRedo
	)
{
	itsIsRedoFlag = isRedo;
}

#endif
