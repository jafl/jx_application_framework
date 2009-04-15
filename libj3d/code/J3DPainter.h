/******************************************************************************
 J3DPainter.h

	Interface for the J3DPainter class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J3DPainter
#define _H_J3DPainter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JColormap.h>

class J3DPainter
{
public:

	J3DPainter(const JColormap& colormap);

	virtual ~J3DPainter();

	const JColormap&	GetColormap() const;

	JColorIndex	GetVertexColor() const;
	void		SetVertexColor(const JColorIndex color);

protected:

	virtual void	HWSetVertexColor(const JColormap& colormap,
									 const JColorIndex color) = 0;

private:

	const JColormap&	itsColormap;
	JColorIndex			itsVertexColor;

private:

	// not allowed

	J3DPainter(const J3DPainter& source);
	const J3DPainter& operator=(const J3DPainter& source);
};


/******************************************************************************
 GetColormap

 ******************************************************************************/

inline const JColormap&
J3DPainter::GetColormap()
	const
{
	return itsColormap;
}

/******************************************************************************
 Vertex color

 ******************************************************************************/

inline JColorIndex
J3DPainter::GetVertexColor()
	const
{
	return itsVertexColor;
}

inline void
J3DPainter::SetVertexColor
	(
	const JColorIndex color
	)
{
	if (color != itsVertexColor)
		{
		itsVertexColor = color;
		HWSetVertexColor(itsColormap, color);
		}
}

#endif
