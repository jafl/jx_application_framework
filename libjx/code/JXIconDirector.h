/******************************************************************************
 JXIconDirector.h

	Interface for the JXIconDirector class

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXIconDirector
#define _H_JXIconDirector

#include "JXWindowDirector.h"

class JXImage;
class JXWindowIcon;

class JXIconDirector : public JXWindowDirector
{
public:

	JXIconDirector(JXWindowDirector* supervisor, JXImage* normalIcon,
				   JXImage* dropIcon);

	virtual ~JXIconDirector();

	JXWindowIcon*	GetIconWidget() const;

private:

	JXWindowIcon*	itsIconWidget;
};


/******************************************************************************
 GetIconWidget

 ******************************************************************************/

inline JXWindowIcon*
JXIconDirector::GetIconWidget()
	const
{
	return itsIconWidget;
}

#endif
