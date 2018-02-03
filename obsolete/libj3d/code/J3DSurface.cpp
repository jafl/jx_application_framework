/******************************************************************************
 J3DSurface.cpp

	Draws a surface based on a staggered grid of points.

	The data is passed in a single JMatrix because it is too inefficient to
	store a 2D array of JVectors, especially since we may get a *lot* of
	points.  Each row is a data point.

	The data must be a staggered grid grouped by x value.  The columns of
	points must contain yCount, yCount-1, yCount, ..., yCount-1, yCount
	values, respectively.

	ComputeGrid() sets up the appropriate grid for you.

	BASE CLASS = J3DObject

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include <J3DSurface.h>
#include <JMatrix.h>
#include <GL/gl.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DSurface::J3DSurface
	(
	J3DObject* parent
	)
	:
	J3DObject(parent, 0,0,0),
	itsXCount(0),
	itsYCount(0),
	itsVertexList(NULL)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DSurface::~J3DSurface()
{
	delete itsVertexList;
}

/******************************************************************************
 ComputeGrid (static)

 ******************************************************************************/

JMatrix*
J3DSurface::ComputeGrid
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const JSize		xCount,
	const JFloat	yMin,
	const JFloat	yMax,
	const JSize		yCount
	)
{
	const JSize ptCount = xCount*yCount + (xCount-1)*(yCount-1);

	JMatrix* data = new JMatrix(ptCount, 3);
	assert( data != NULL );

	JIndex rowIndex = 1;

	const JFloat  xDelta = (xMax - xMin) / (xCount-1);
	const JFloat  yDelta = (yMax - yMin) / (yCount-1);
	const JFloat xOffset = xDelta/2;
	const JFloat yOffset = yDelta/2;

	for (JIndex xIndex=1; xIndex<=xCount; xIndex++)
		{
		JFloat x = xMin + xDelta*(xIndex-1);

		for (JIndex yIndex=1; yIndex<=yCount; yIndex++)
			{
			const JFloat y = yMin + yDelta*(yIndex-1);

			data->SetElement(rowIndex, 1, x);
			data->SetElement(rowIndex, 2, y);
			rowIndex++;
			}

		if (xIndex < xCount)
			{
			x += xOffset;

			for (JIndex yIndex=1; yIndex<yCount; yIndex++)
				{
				const JFloat y = yMin + yOffset + yDelta*(yIndex-1);

				data->SetElement(rowIndex, 1, x);
				data->SetElement(rowIndex, 2, y);
				rowIndex++;
				}
			}
		}

	assert( rowIndex == ptCount+1 );

	return data;
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
J3DSurface::SetData
	(
	const JSize	xCount,
	const JSize	yCount,
	JMatrix*	data
	)
{
	itsXCount = xCount;
	itsYCount = yCount;

	delete itsVertexList;
	itsVertexList = data;

	itsZMin = itsZMax = itsVertexList->GetElement(1, 3);

	const JSize count = itsVertexList->GetRowCount();
	for (JIndex i=2; i<=count; i++)
		{
		const JFloat z = itsVertexList->GetElement(i, 3);
		if (z < itsZMin)
			{
			itsZMin = z;
			}
		if (z > itsZMax)
			{
			itsZMax = z;
			}
		}

	Refresh();
}

/******************************************************************************
 Render (virtual protected)

 ******************************************************************************/

void
J3DSurface::Render
	(
	J3DPainter& p
	)
{
	if (itsXCount == 0 || itsYCount == 0 || itsVertexList == NULL)
		{
		return;
		}

	glBegin(GL_TRIANGLE_STRIP);

	for (JIndex xIndex=1; xIndex<itsXCount; xIndex++)
		{
		JIndex rowIndexOffset = (xIndex-1)*(2*itsYCount-1);

		JIndex rowIndex = 1 + xIndex*(2*itsYCount-1);
		PlaceVertex(p, rowIndex);

		for (JIndex yIndex=1; yIndex<=itsYCount; yIndex++)
			{
			JIndex rowIndex = rowIndexOffset + yIndex;
			PlaceVertex(p, rowIndex);

			if (yIndex < itsYCount)
				{
				rowIndex += itsYCount;
				PlaceVertex(p, rowIndex);
				}
			}

		rowIndexOffset = xIndex*(2*itsYCount-1);
		JIndex j       = 0;
		for (JIndex yIndex=itsYCount; yIndex>=1; yIndex--)
			{
			JIndex rowIndex = rowIndexOffset + yIndex;
			PlaceVertex(p, rowIndex);

			if (yIndex > 1)
				{
				rowIndex = rowIndexOffset - j;
				PlaceVertex(p, rowIndex);
				}

			j++;
			}
		}

	glEnd();
}

/******************************************************************************
 PlaceVertex (private)

 ******************************************************************************/

void
J3DSurface::PlaceVertex
	(
	J3DPainter&		p,
	const JIndex	rowIndex
	)
{
	const JFloat vertex[3] =
		{
		itsVertexList->GetElement(rowIndex, 1),
		itsVertexList->GetElement(rowIndex, 2),
		itsVertexList->GetElement(rowIndex, 3)
		};

	JRGB color;
	JFloat alpha = (vertex[2] - itsZMin)/(itsZMax - itsZMin);
	if (alpha <= 0.5)
		{
		color = JBlend(itsMinColor, itsMidColor, 2.0*alpha);
		}
	else
		{
		color = JBlend(itsMidColor, itsMaxColor, 2.0*alpha - 1.0);
		}
	const JColorIndex colorIndex = (p.GetColormap()).GetColor(color);
	p.SetVertexColor(colorIndex);

	glVertex3dv(vertex);
}
