/******************************************************************************
 GLUndoElementAppend.h

	Interface for the GLUndoElementAppend class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementAppend
#define _H_GLUndoElementAppend

#include <GLUndoElementBase.h>
#include <JPoint.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoElementAppend : public GLUndoElementBase
{
public:

	GLUndoElementAppend(GXRaggedFloatTable* table, const JPoint& cell);

	virtual ~GLUndoElementAppend();

	virtual void	Undo();

private:

private:

	// not allowed

	GLUndoElementAppend(const GLUndoElementAppend& source);
	const GLUndoElementAppend& operator=(const GLUndoElementAppend& source);
};

#endif
