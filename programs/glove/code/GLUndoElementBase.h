/******************************************************************************
 GLUndoElementBase.h

	Interface for the GLUndoElementBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementBase
#define _H_GLUndoElementBase

#include <GLUndoBase.h>
#include <JPoint.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoElementBase : public GLUndoBase
{
public:

	GLUndoElementBase(GXRaggedFloatTable* table, const JPoint& cell);

	virtual ~GLUndoElementBase();

protected:

	JPoint					GetCell();

private:

	JPoint					itsCell;

private:

	// not allowed

	GLUndoElementBase(const GLUndoElementBase& source);
	const GLUndoElementBase& operator=(const GLUndoElementBase& source);
};

/******************************************************************************
 GetCell (protected)

 ******************************************************************************/

inline JPoint
GLUndoElementBase::GetCell()
{
	return itsCell;
}

#endif
