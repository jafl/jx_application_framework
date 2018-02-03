/******************************************************************************
 J3DLine.h

	Interface for the J3DLine class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_J3DLine
#define _H_J3DLine

#include <J3DObject.h>

class J3DLine : public J3DObject
{
public:

	enum VertexIndex
	{
		kStartVertex,
		kEndVertex
	};

public:

	J3DLine(J3DObject* parent,
			const JFloat x1, const JFloat y1, const JFloat z1,
			const JFloat x2, const JFloat y2, const JFloat z2);
	J3DLine(J3DObject* parent, const JVector& start, const JVector& end);

	virtual ~J3DLine();

	JVector	GetVertex(const VertexIndex index) const;
	void	SetVertex(const VertexIndex index,
					  const JFloat x, const JFloat y, const JFloat z);
	void	SetVertex(const VertexIndex index, const JVector& vertex);

	JColorIndex	GetVertexColor(const VertexIndex index) const;
	void		SetVertexColor(const VertexIndex index, const JColorIndex color);
	void		SetColor(const JColorIndex color);

protected:

	virtual void	Render(J3DPainter& p);

private:

	JVector		itsLineVector;
	JColorIndex itsVertexColor[2];

private:

	void	J3DLineX();

	// not allowed

	J3DLine(const J3DLine& source);
	const J3DLine& operator=(const J3DLine& source);
};

#endif
