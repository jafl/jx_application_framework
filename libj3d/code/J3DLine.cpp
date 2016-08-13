/******************************************************************************
 J3DLine.cpp

	The line's position is its starting point.  We store the difference
	between start and end so the line can be moved anywhere with
	SetPosition().

	BASE CLASS = J3DObject

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <J3DLine.h>
#include <GL/gl.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DLine::J3DLine
	(
	J3DObject*		parent,
	const JFloat	x1,
	const JFloat	y1,
	const JFloat	z1,
	const JFloat	x2,
	const JFloat	y2,
	const JFloat	z2
	)
	:
	J3DObject(parent, x1,y1,z1),
	itsLineVector(3, x2-x1, y2-y1, z2-z1)
{
	J3DLineX();
}

J3DLine::J3DLine
	(
	J3DObject*		parent,
	const JVector&	start,
	const JVector&	end
	)
	:
	J3DObject(parent, start),
	itsLineVector(end - start)
{
	J3DLineX();
}

// private

void
J3DLine::J3DLineX()
{
	itsVertexColor[0] = itsVertexColor[1] = GetDefaultColor();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DLine::~J3DLine()
{
}

/******************************************************************************
 Vertex

 ******************************************************************************/

JVector
J3DLine::GetVertex
	(
	const VertexIndex index
	)
	const
{
	assert( index < 2 );
	if (index == kStartVertex)
		{
		return GetPosition();
		}
	else
		{
		return GetPosition() + itsLineVector;
		}
}

void
J3DLine::SetVertex
	(
	const VertexIndex	index,
	const JFloat		x,
	const JFloat		y,
	const JFloat		z
	)
{
	SetVertex(index, JVector(3,x,y,z));
}

void
J3DLine::SetVertex
	(
	const VertexIndex	index,
	const JVector&		vertex
	)
{
	assert( index < 2 );
	if (index == kStartVertex)
		{
		SetPosition(vertex);
		}
	else
		{
		itsLineVector = vertex - GetPosition();
		}
	Refresh();
}

/******************************************************************************
 Vertex color

 ******************************************************************************/

JColorIndex
J3DLine::GetVertexColor
	(
	const VertexIndex index
	)
	const
{
	assert( index < 2 );
	return itsVertexColor [ index ];
}

void
J3DLine::SetVertexColor
	(
	const VertexIndex	index,
	const JColorIndex	color
	)
{
	assert( index < 2 );
	itsVertexColor [ index ] = color;
	Refresh();
}

void
J3DLine::SetColor
	(
	const JColorIndex color
	)
{
	itsVertexColor[0] = color;
	itsVertexColor[1] = color;
	Refresh();
}

/******************************************************************************
 Render (virtual protected)

 ******************************************************************************/

void
J3DLine::Render
	(
	J3DPainter& p
	)
{
	glBegin(GL_LINES);

	p.SetVertexColor(itsVertexColor [ kStartVertex ]);
	glVertex3f(0.0, 0.0, 0.0);

	p.SetVertexColor(itsVertexColor [ kEndVertex ]);
	glVertex3dv(itsLineVector.GetElements());

	glEnd();
}
