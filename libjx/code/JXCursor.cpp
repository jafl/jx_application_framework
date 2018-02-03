/******************************************************************************
 JXCursor.cpp

	Useful cursors that are not provided by X.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXCursor.h>
#include <JXDisplay.h>

/******************************************************************************
 Invisible cursor

 ******************************************************************************/

static const JUtf8Byte* kInvisibleCursorName = "JXInvisible";

static char invisible_cursor_bits[]      = {0x00};
static char invisible_cursor_mask_bits[] = {0x00};

static const JXCursor kInvisibleCursor =
{
	1,1, 0,0, invisible_cursor_bits, invisible_cursor_mask_bits
};	

JCursorIndex
JXGetInvisibleCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kInvisibleCursorName, kInvisibleCursor);
}

/******************************************************************************
 Hand cursor

 ******************************************************************************/

static const JUtf8Byte* kHandCursorName = "JXHand";

#include <jx_hand_cursor.xbm>
#include <jx_hand_cursor_mask.xbm>

static const JXCursor kHandCursor =
{
	jx_hand_cursor_width, jx_hand_cursor_height, 7,8,
	jx_hand_cursor_bits, jx_hand_cursor_mask_bits
};

JCursorIndex
JXGetHandCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kHandCursorName, kHandCursor);
}

/******************************************************************************
 Drag horiz line cursor

 ******************************************************************************/

static const JUtf8Byte* kDragHorizLineCursorName = "JXDragHorizLine";

#include <jx_drag_horiz_line_cursor.xbm>
#include <jx_drag_horiz_line_cursor_mask.xbm>

static const JXCursor kDragHorizLineCursor =
{
	jx_drag_horiz_line_cursor_width, jx_drag_horiz_line_cursor_height, 8,8,
	jx_drag_horiz_line_cursor_bits, jx_drag_horiz_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragHorizLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragHorizLineCursorName,
									   kDragHorizLineCursor);
}

/******************************************************************************
 Drag all horiz line cursor

 ******************************************************************************/

static const JUtf8Byte* kDragAllHorizLineCursorName = "JXDragAllHorizLine";

#include <jx_drag_all_horiz_line_cursor.xbm>
#include <jx_drag_all_horiz_line_cursor_mask.xbm>

static const JXCursor kDragAllHorizLineCursor =
{
	jx_drag_all_horiz_line_cursor_width, jx_drag_all_horiz_line_cursor_height, 8,8,
	jx_drag_all_horiz_line_cursor_bits, jx_drag_all_horiz_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragAllHorizLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragAllHorizLineCursorName,
									   kDragAllHorizLineCursor);
}

/******************************************************************************
 Drag vert line cursor

 ******************************************************************************/

static const JUtf8Byte* kDragVertLineCursorName = "JXDragVertLine";

#include <jx_drag_vert_line_cursor.xbm>
#include <jx_drag_vert_line_cursor_mask.xbm>

static const JXCursor kDragVertLineCursor =
{
	jx_drag_vert_line_cursor_width, jx_drag_vert_line_cursor_height, 8,8,
	jx_drag_vert_line_cursor_bits, jx_drag_vert_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragVertLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragVertLineCursorName,
									   kDragVertLineCursor);
}

/******************************************************************************
 Drag all vert line cursor

 ******************************************************************************/

static const JUtf8Byte* kDragAllVertLineCursorName = "JXDragAllVertLine";

#include <jx_drag_all_vert_line_cursor.xbm>
#include <jx_drag_all_vert_line_cursor_mask.xbm>

static const JXCursor kDragAllVertLineCursor =
{
	jx_drag_all_vert_line_cursor_width, jx_drag_all_vert_line_cursor_height, 8,8,
	jx_drag_all_vert_line_cursor_bits, jx_drag_all_vert_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragAllVertLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragAllVertLineCursorName,
									   kDragAllVertLineCursor);
}

/******************************************************************************
 Drag file

 ******************************************************************************/

static const JUtf8Byte* kDragFileCursorName = "JXDragFile";

#include <jx_drag_file_cursor.xbm>
#include <jx_drag_file_cursor_mask.xbm>

static const JXCursor kDragFileCursor =
{
	jx_drag_file_cursor_width, jx_drag_file_cursor_height, 6,7,
	jx_drag_file_cursor_bits, jx_drag_file_cursor_mask_bits
};	

JCursorIndex
JXGetDragFileCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragFileCursorName,
									   kDragFileCursor);
}

/******************************************************************************
 Drag directory

 ******************************************************************************/

static const JUtf8Byte* kDragDirectoryCursorName = "JXDragDirectory";

#include <jx_drag_directory_cursor.xbm>
#include <jx_drag_directory_cursor_mask.xbm>

static const JXCursor kDragDirectoryCursor =
{
	jx_drag_directory_cursor_width, jx_drag_directory_cursor_height, 6,7,
	jx_drag_directory_cursor_bits, jx_drag_directory_cursor_mask_bits
};	

JCursorIndex
JXGetDragDirectoryCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragDirectoryCursorName,
									   kDragDirectoryCursor);
}

/******************************************************************************
 Drag file and directory

 ******************************************************************************/

static const JUtf8Byte* kDragFileAndDirectoryCursorName = "JXDragFileAndDirectory";

#include <jx_drag_file_and_directory_cursor.xbm>
#include <jx_drag_file_and_directory_cursor_mask.xbm>

static const JXCursor kDragFileAndDirectoryCursor =
{
	jx_drag_file_and_directory_cursor_width, jx_drag_file_and_directory_cursor_height, 6,7,
	jx_drag_file_and_directory_cursor_bits, jx_drag_file_and_directory_cursor_mask_bits
};	

JCursorIndex
JXGetDragFileAndDirectoryCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragFileAndDirectoryCursorName,
									   kDragFileAndDirectoryCursor);
}
