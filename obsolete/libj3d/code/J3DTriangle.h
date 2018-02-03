/******************************************************************************
 J3DTriangle.h

	Interface for the J3DTriangle class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_J3DTriangle
#define _H_J3DTriangle

#include <J3DObject.h>

class J3DTriangle : public J3DObject
{
public:

	J3DTriangle(J3DObject* parent,
				const JVector& v1, const JVector& v2, const JVector& v3);

	virtual ~J3DTriangle();

	const JVector&	GetVertex(const JIndex index) const;
	void			SetVertex(const JIndex index,
							  const JFloat x, const JFloat y, const JFloat z);
	void			SetVertex(const JIndex index, const JVector& vertex);

	JColorIndex	GetVertexColor(const JIndex index) const;
	void		SetVertexColor(const JIndex index, const JColorIndex color);
	void		SetColor(const JColorIndex color);

	JBoolean	IndexValid(const JIndex index) const;

protected:

	virtual void	Render(J3DPainter& p);

private:

	JVector		itsVertex1;
	JVector		itsVertex2;
	JVector		itsVertex3;
	JVector*	itsVertex[3];		// pointers to itsVertexN
	JColorIndex itsVertexColor[3];

private:

	void	J3DTriangleX();

	// not allowed

	J3DTriangle(const J3DTriangle& source);
	const J3DTriangle& operator=(const J3DTriangle& source);
};


/******************************************************************************
 IndexValid

 ******************************************************************************/

inline JBoolean
J3DTriangle::IndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= 3 );
}

#endif
