/******************************************************************************
 JXCursor.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCursor
#define _H_JXCursor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <X11/Xlib.h>

class JXDisplay;

typedef JIndex	JCursorIndex;

// predefined by every JXDisplay object

enum
{
	kJXDefaultCursor = 1,	// XC_left_ptr
	kJXTextEditCursor,		// XC_xterm
	kJXCrosshairCursor,		// XC_tcross
	kJXBusyCursor,			// XC_watch
	kJXInactiveCursor		// XC_X_cursor
};

// data structure for creating custom cursors

struct JXCursor
{
	JCoordinate		w,h;
	JCoordinate		hotx,hoty;
	char*			data;
	char*			mask;
};

// custom cursors provided by JX

JCursorIndex JXGetInvisibleCursor(JXDisplay* display);				// JXInvisible

JCursorIndex JXGetHandCursor(JXDisplay* display);					// JXHand

JCursorIndex JXGetDragHorizLineCursor(JXDisplay* display);			// JXDragHorizLine
JCursorIndex JXGetDragAllHorizLineCursor(JXDisplay* display);		// JXDragAllHorizLine

JCursorIndex JXGetDragVertLineCursor(JXDisplay* display);			// JXDragVertLine
JCursorIndex JXGetDragAllVertLineCursor(JXDisplay* display);		// JXDragAllVertLine

JCursorIndex JXGetDragFileCursor(JXDisplay* display);				// JXDragFile
JCursorIndex JXGetDragDirectoryCursor(JXDisplay* display);			// JXDragDirectory
JCursorIndex JXGetDragFileAndDirectoryCursor(JXDisplay* display);	// JXDragFileAndDirectory

#endif
