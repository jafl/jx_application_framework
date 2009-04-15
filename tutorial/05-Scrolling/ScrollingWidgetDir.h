/******************************************************************************
 ScrollingWidgetDir.h

	Interface for the ScrollingWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_ScrollingWidgetDir
#define _H_ScrollingWidgetDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class ScrollingWidgetDir : public JXWindowDirector
{
public:

	ScrollingWidgetDir(JXDirector* supervisor);

	virtual ~ScrollingWidgetDir();

private:

	void BuildWindow();

	// not allowed

	ScrollingWidgetDir(const ScrollingWidgetDir& source);
	const ScrollingWidgetDir& operator=(const ScrollingWidgetDir& source);
};

#endif
