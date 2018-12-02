/******************************************************************************
 GLUndoBase.h

	Interface for the GLUndoBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoBase
#define _H_GLUndoBase

#include <JUndo.h>
#include <JPoint.h>

class GLRaggedFloatTable;
class GLRaggedFloatTableData;

class GLUndoBase : public JUndo
{
public:

	GLUndoBase(GLRaggedFloatTable* table);

	virtual ~GLUndoBase();

protected:

	GLRaggedFloatTableData*	GetData();
	GLRaggedFloatTable*		GetTable();
	void					NewUndo(JUndo* undo);

private:

	GLRaggedFloatTableData*	itsData;	// we don't own this
	GLRaggedFloatTable*		itsTable;	// we don't own this

private:

	// not allowed

	GLUndoBase(const GLUndoBase& source);
	const GLUndoBase& operator=(const GLUndoBase& source);
};

/******************************************************************************
 GetData (protected)

 ******************************************************************************/

inline GLRaggedFloatTableData*
GLUndoBase::GetData()
{
	return itsData;
}

/******************************************************************************
 GetTable (protected)

 ******************************************************************************/

inline GLRaggedFloatTable*
GLUndoBase::GetTable()
{
	return itsTable;
}

#endif
