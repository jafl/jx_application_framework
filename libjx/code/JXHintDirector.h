/******************************************************************************
 JXHintDirector.h

	Copyright © 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHintDirector
#define _H_JXHintDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JRect.h>

class JXContainer;

class JXHintDirector : public JXWindowDirector
{
public:

	JXHintDirector(JXDirector* supervisor, JXContainer* widget,
				   const JCharacter* text);
	JXHintDirector(JXDirector* supervisor, JXContainer* widget,
				   const JRect& rect, const JCharacter* text);

	virtual	~JXHintDirector();

private:

	void	BuildWindow(JXColormap* colormap, const JRect& frameR,
						const JCharacter* text);

	// not allowed

	JXHintDirector(const JXHintDirector& source);
	const JXHintDirector& operator=(const JXHintDirector& source);
};

#endif
