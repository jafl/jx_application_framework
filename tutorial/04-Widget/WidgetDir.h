/******************************************************************************
 WidgetDir.h

	Interface for the WidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_WidgetDir
#define _H_WidgetDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class WidgetDir : public JXWindowDirector
{
public:

	WidgetDir(JXDirector* supervisor);

	virtual ~WidgetDir();

private:

	void BuildWindow();

	// not allowed

	WidgetDir(const WidgetDir& source);
	const WidgetDir& operator=(const WidgetDir& source);
};

#endif
