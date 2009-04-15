/******************************************************************************
 J3DTriangle.cpp

	The coordinates of all 3 vertices are relative to the object's position.
	The initial position is the origin, rather than one of the vertices,
	to avoid breaking the symmetry.

	We store itsVertex[] separately because C++ does not allow construction
	of an array of objects without a default constructor.

	BASE CLASS = J3DObject

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <J3DTriangle.h>
#include <GL/gl.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DTriangle::J3DTriangle
	(
	J3DObject*		parent,
	const JVector&	v1,
	const JVector&	v2,
	const JVector&	v3
	)
	:
	J3DObject(parent, 0,0,0),
	itsVertex1(v1),
	itsVertex2(v2),
	itsVertex3(v3)
{
	J3DTriangleX();
}

// private

void
J3DTriangle::J3DTriangleX()
{
	itsVertexColor[0] = itsVertexColor[1] =
	itsVertexColor[2] = GetDefaultColor();

	itsVertex[0] = &itsVertex1;
	itsVertex[1] = &itsVertex2;
	itsVertex[2] = &itsVertex3;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DTriangle::~J3DTriangle()
{
}

/******************************************************************************
 Vertex

 ******************************************************************************/

const JVector&
J3DTriangle::GetVertex
	(
	const JIndex index
	)
	const
{
	assert( IndexValid(index) );
	return *itsVertex [ index-1 ];
}

void
J3DTriangle::SetVertex
	(
	const JIndex	index,
	const JFloat	x,
	const JFloat	y,
	const JFloat	z
	)
{
	assert( IndexValid(index) );
	itsVertex [ index-1 ] -> Set(x,y,z);
	Refresh();
}

void
J3DTriangle::SetVertex
	(
	const JIndex	index,
	const JVector&	vertex
	)
{
	assert( IndexValid(index) );
	*itsVertex [ index-1 ] = vertex;
	Refresh();
}

/******************************************************************************
 Vertex color

 ******************************************************************************/

JColorIndex
J3DTriangle::GetVertexColor
	(
	const JIndex index
	)
	const
{
	assert( IndexValid(index) );
	return itsVertexColor [ index-1 ];
}

void
J3DTriangle::SetVertexColor
	(
	const JIndex		index,
	const JColorIndex	color
	)
{
	assert( IndexValid(index) );
	itsVertexColor [ index-1 ] = color;
	Refresh();
}

void
J3DTriangle::SetColor
	(
	const JColorIndex color
	)
{
	itsVertexColor[0] = itsVertexColor[1] =
	itsVertexColor[2] = color;
	Refresh();
}

/******************************************************************************
 Render (virtual protected)

 ******************************************************************************/

void
J3DTriangle::Render
	(
	J3DPainter& p
	)
{
	glBegin(GL_TRIANGLES);

	for (JIndex i=0; i<3; i++)
		{
		p.SetVertexColor(itsVertexColor[i]);
		glVertex3dv(itsVertex[i]->GetElements());
		}

	glEnd();
}
