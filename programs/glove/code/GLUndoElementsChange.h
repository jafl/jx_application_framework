/******************************************************************************
 GLUndoElementsChange.h

	Interface for the GLUndoElementsChange class.

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementsChange
#define _H_GLUndoElementsChange

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <GLUndoElementsBase.h>
#include <JPoint.h>
#include <JPtrArray.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoElementsChange : public GLUndoElementsBase
{
public:

	GLUndoElementsChange(GXRaggedFloatTable* table, const JPoint& start,
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
