/******************************************************************************
 J3DTetrahedron.h

	Interface for the J3DTetrahedron class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_J3DTetrahedron
#define _H_J3DTetrahedron

#include <J3DObject.h>

class J3DTetrahedron : public J3DObject
{
public:

	enum VertexIndex
	{
		kTopVertex,
		kFrontVertex,
		kBackLeftVertex,
		kBackRightVertex
	};

public:

	J3DTetrahedron(J3DObject* parent, const JFloat x, const JFloat y, const JFloat z);
	J3DTetrahedron(J3DObject* parent, const JVector& position);

	virtual ~J3DTetrahedron();

	JColorIndex	GetVertexColor(const VertexIndex vertex) const;
	void		SetVertexColor(const VertexIndex vertex, const JColorIndex color);

protected:

	virtual void	Render(J3DPainter& p);

private:

	JColorIndex itsVertexColor[4];

private:

	void	J3DTetrahedronX();

	// not allowed

	J3DTetrahedron(const J3DTetrahedron& source);
	const J3DTetrahedron& operator=(const J3DTetrahedron& source);
};

#endif
