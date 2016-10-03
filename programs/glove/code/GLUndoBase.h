/******************************************************************************
 GLUndoBase.h

	Interface for the GLUndoBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoBase
#define _H_GLUndoBase

#include <JUndo.h>
#include <JPoint.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoBase : public JUndo
{
public:

	GLUndoBase(GXRaggedFloatTable* table);

	virtual ~GLUndoBase();

protected:

	GRaggedFloatTableData*	GetData();
	GXRaggedFloatTable*		GetTable();
	void					NewUndo(JUndo* undo);

private:

	GRaggedFloatTableData*	itsData;	// we don't own this
	GXRaggedFloatTable*		itsTable;	// we don't own this

private:

	// not allowed

	GLUndoBase(const GLUndoBase& source);
	const GLUndoBase& operator=(const GLUndoBase& source);
};

/******************************************************************************
 GetData (protected)

 ******************************************************************************/

inline GRaggedFloatTableData*
GLUndoBase::GetData()
{
	return itsData;
}

/******************************************************************************
 GetTable (protected)

 ******************************************************************************/

inline GXRaggedFloatTable*
GLUndoBase::GetTable()
{
	return itsTable;
}

#endif
