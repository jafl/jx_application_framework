/******************************************************************************
 JXHintDirector.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXHintDirector
#define _H_JXHintDirector

#include "JXWindowDirector.h"
#include <jx-af/jcore/JRect.h>

class JXContainer;

class JXHintDirector : public JXWindowDirector
{
public:

	JXHintDirector(JXDirector* supervisor, JXContainer* widget,
				   const JString& text);
	JXHintDirector(JXDirector* supervisor, JXContainer* widget,
				   const JRect& rect, const JString& text);

	~JXHintDirector() override;

private:

	void	BuildWindow(const JRect& frameR, const JString& text);
};

#endif
