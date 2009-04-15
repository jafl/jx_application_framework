/******************************************************************************
 DragWidgetDir.h

	Interface for the DragWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DragWidgetDir
#define _H_DragWidgetDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class DragWidgetDir : public JXWindowDirector
{
public:

	DragWidgetDir(JXDirector* supervisor);

	virtual ~DragWidgetDir();

private:

	void BuildWindow();

	// not allowed

	DragWidgetDir(const DragWidgetDir& source);
	const DragWidgetDir& operator=(const DragWidgetDir& source);
};

#endif
