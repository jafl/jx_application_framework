/******************************************************************************
 GLUndoElementsCut.h

	Interface for the GLUndoElementsCut class.

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementsCut
#define _H_GLUndoElementsCut

#include <GLUndoElementsBase.h>
#include <JPoint.h>
#include <JPtrArray.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoElementsCut : public GLUndoElementsBase
{
public:

	GLUndoElementsCut(GXRaggedFloatTable* table, const JPoint& start,
						 const JPoint& end, 
						 const GLUndoElementsBase::UndoType type);

	virtual ~GLUndoElementsCut();

	virtual void	Undo();

private:

	JPtrArray<JArray<JFloat> >*	itsValues;

private:

	// not allowed

	GLUndoElementsCut(const GLUndoElementsCut& source);
	const GLUndoElementsCut& operator=(const GLUndoElementsCut& source);
};

#endif
