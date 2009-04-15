/******************************************************************************
 GLUndoElementChange.h

	Interface for the GLUndoElementChange class.

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementChange
#define _H_GLUndoElementChange

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
