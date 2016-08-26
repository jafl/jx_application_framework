/******************************************************************************
 GLUndoElementCut.h

	Interface for the GLUndoElementCut class.

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementCut
#define _H_GLUndoElementCut

#include <GLUndoElementBase.h>
#include <JPoint.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoElementCut : public GLUndoElementBase
{
public:

	GLUndoElementCut(GXRaggedFloatTable* table, const JPoint& cell,
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
