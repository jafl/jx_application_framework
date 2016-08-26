/******************************************************************************
 JXDragPainter.h

	Interface for the JXDragPainter class

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDragPainter
#define _H_JXDragPainter

#include <JXWindowPainter.h>

class JXWindow;

class JXDragPainter : public JXWindowPainter
{
public:

	JXDragPainter(JXDisplay* display, JXWindow* window,
				  const JRect& defaultClipRect);

	virtual ~JXDragPainter();

private:

	// not allowed

	JXDragPainter(const JXDragPainter& source);
	const JXDragPainter& operator=(const JXDragPainter& source);
};

#endif
