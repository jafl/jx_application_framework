/******************************************************************************
 GLUndoElementCut.h

	Interface for the GLUndoElementCut class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementCut
#define _H_GLUndoElementCut

#include <GLUndoElementBase.h>
#include <JPoint.h>

class GLRaggedFloatTable;
class GLRaggedFloatTableData;

class GLUndoElementCut : public GLUndoElementBase
{
public:

	GLUndoElementCut(GLRaggedFloatTable* table, const JPoint& cell,
					 const JFloat value);

	virtual ~GLUndoElementCut();

	virtual void	Undo();

private:

	JFloat	itsValue;

private:

	// not allowed

	GLUndoElementCut(const GLUndoElementCut& source);
	const GLUndoElementCut& operator=(const GLUndoElementCut& source);
};

#endif
