/******************************************************************************
 GLUndoElementChange.h

	Interface for the GLUndoElementChange class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementChange
#define _H_GLUndoElementChange

#include <GLUndoElementBase.h>
#include <JPoint.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoElementChange : public GLUndoElementBase
{
public:

	GLUndoElementChange(GXRaggedFloatTable* table, const JPoint& cell,
						const JFloat value);

	virtual ~GLUndoElementChange();

	virtual void	Undo();

private:

	JFloat	itsValue;

private:

	// not allowed

	GLUndoElementChange(const GLUndoElementChange& source);
	const GLUndoElementChange& operator=(const GLUndoElementChange& source);
};

#endif
