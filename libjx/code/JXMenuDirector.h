/******************************************************************************
 JXMenuDirector.h

	Interface for the JXMenuDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenuDirector
#define _H_JXMenuDirector

#include "JXWindowDirector.h"
#include <JRect.h>

class JXMenu;
class JXMenuTable;

class JXMenuDirector : public JXWindowDirector
{
public:

	JXMenuDirector(JXDirector* supervisor, JXMenu* menu);

	virtual ~JXMenuDirector();

	JXMenuTable*	GetMenuTable() const;

	// must be called immediately after constructing

	void	BuildWindow(const JPoint& leftPtR, const JPoint& rightPtR,
						const JCoordinate sourceHeight);
	void	GrabKeyboard();

	// only call when activated by shortcut

	void	GrabPointer();

protected:

	virtual JXMenuTable*	CreateMenuTable() = 0;

private:

	JXMenuTable*	itsMenuTable;

private:

	// not allowed

	JXMenuDirector(const JXMenuDirector& source);
	const JXMenuDirector& operator=(const JXMenuDirector& source);
};


/******************************************************************************
 GetMenuTable

 ******************************************************************************/

inline JXMenuTable*
JXMenuDirector::GetMenuTable()
	const
{
	return itsMenuTable;
}

#endif
