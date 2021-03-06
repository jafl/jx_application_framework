/******************************************************************************
 JXIconDirector.h

	Interface for the JXIconDirector class

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXIconDirector
#define _H_JXIconDirector

#include <JXWindowDirector.h>

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

private:

	// not allowed

	JXIconDirector(const JXIconDirector& source);
	const JXIconDirector& operator=(const JXIconDirector& source);
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
