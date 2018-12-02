/******************************************************************************
 GLUndoElementsChange.h

	Interface for the GLUndoElementsChange class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementsChange
#define _H_GLUndoElementsChange

#include <GLUndoElementsBase.h>
#include <JPoint.h>
#include <JPtrArray.h>

class GLRaggedFloatTable;
class GLRaggedFloatTableData;

class GLUndoElementsChange : public GLUndoElementsBase
{
public:

	GLUndoElementsChange(GLRaggedFloatTable* table, const JPoint& start,
						 const JPoint& end, 
						 const GLUndoElementsBase::UndoType type);

	virtual ~GLUndoElementsChange();

	virtual void	Undo();

private:

	JPtrArray<JArray<JFloat> >*	itsValues;

private:

	// not allowed

	GLUndoElementsChange(const GLUndoElementsChange& source);
	const GLUndoElementsChange& operator=(const GLUndoElementsChange& source);
};

#endif
