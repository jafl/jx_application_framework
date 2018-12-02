/******************************************************************************
 GLUndoElementsInsert.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = GLUndoElementsBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoElementsInsert.h>
#include <GLUndoElementsCut.h>
#include <GLRaggedFloatTable.h>
#include <GLRaggedFloatTableData.h>

#include <JMinMax.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementsInsert::GLUndoElementsInsert
	(
	GLRaggedFloatTable* 				table,
	const JPoint&						start,
	const JPoint&						end,
	const GLUndoElementsBase::UndoType	type
	)
	:
	GLUndoElementsBase(table, start, end, type)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLUndoElementsInsert::~GLUndoElementsInsert()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
GLUndoElementsInsert::Undo()
{

	// we need to create this before we change the data, because
	// it needs to read the old data first. We can't yet call NewUndo, 
	// though, because that will delete us.

	GLUndoElementsCut* undo =
		jnew GLUndoElementsCut(GetTable(), GetStartCell(), GetEndCell(), GetType());
	assert(undo != nullptr);
	
	GLRaggedFloatTableData* data 		= GetData();
	JPoint start 						= GetStartCell();
	JPoint end 							= GetEndCell();
	GLUndoElementsBase::UndoType type 	= GetType();

	if (type == GLUndoElementsBase::kRows)
		{
		for (JCoordinate i = start.y; i <= end.y; i++)
			{
			data->RemoveRow(start.y);
			}
		}
	else if (type == GLUndoElementsBase::kCols)
		{
		for (JCoordinate i = start.x; i <= end.x; i++)
			{
			data->RemoveCol(start.x);
			}
		}
	else if (type == GLUndoElementsBase::kElements)
		{
		for (JCoordinate i = start.x; i <= end.x; i++)
			{
			for (JCoordinate j = start.y; j <= end.y; j++)
				{
				if (data->CellValid(start.y, i))
					{
					data->RemoveElement(start.y, i);
					}
				}
			}
		}
		
	NewUndo(undo);
}
