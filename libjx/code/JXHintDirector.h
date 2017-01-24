/******************************************************************************
 JXHintDirector.h

	Copyright (C) 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHintDirector
#define _H_JXHintDirector

#include <JXWindowDirector.h>
#include <JRect.h>

class JXContainer;

class JXHintDirector : public JXWindowDirector
{
public:

	JXHintDirector(JXDirector* supervisor, JXContainer* widget,
				   const JString& text);
	JXHintDirector(JXDirector* supervisor, JXContainer* widget,
				   const JRect& rect, const JString& text);

	virtual	~JXHintDirector();

private:

	void	BuildWindow(const JRect& frameR, const JString& text);

	// not allowed

	JXHintDirector(const JXHintDirector& source);
	const JXHintDirector& operator=(const JXHintDirector& source);
};

#endif
