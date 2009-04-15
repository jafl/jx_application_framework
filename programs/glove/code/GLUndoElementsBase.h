/******************************************************************************
 GLUndoElementsBase.h

	Interface for the GLUndoElementsBase class.

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementsBase
#define _H_GLUndoElementsBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <GLUndoBase.h>
#include <JPoint.h>

class GXRaggedFloatTable;
class GRaggedFloatTableData;

class GLUndoElementsBase : public GLUndoBase
{
public:

	enum UndoType
		{
		kRows = 1,
		kCols,
		kElements
		};
	
public:

	GLUndoElementsBase(GXRaggedFloatTable* table, const JPoint& start,
					   const JPoint& end, const UndoType type);

	virtual ~GLUndoElementsBase();

protected:

	JPoint					GetStartCell();
	JPoint					GetEndCell();
	UndoType				GetType();

private:

	JPoint					itsStartCell;
	JPoint					itsEndCell;
	UndoType				itsType;

private:

	// not allowed

	GLUndoElementsBase(const GLUndoElementsBase& source);
	const GLUndoElementsBase& operator=(const GLUndoElementsBase& source);
};

/******************************************************************************
 GetStartCell (protected)

 ******************************************************************************/

inline JPoint
GLUndoElementsBase::GetStartCell()
{
	return itsStartCell;
}

/******************************************************************************
 GetEndCell (protected)

 ******************************************************************************/

inline JPoint
GLUndoElementsBase::GetEndCell()
{
	return itsEndCell;
}

/******************************************************************************
 GetType (protected)

 ******************************************************************************/

inline GLUndoElementsBase::UndoType
GLUndoElementsBase::GetType()
{
	return itsType;
}

#endif
