/******************************************************************************
 J3DSurface.h

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J3DSurface
#define _H_J3DSurface

#include <J3DObject.h>

class JMatrix;

class J3DSurface : public J3DObject
{
public:

	J3DSurface(J3DObject* parent);

	virtual ~J3DSurface();

	static JMatrix*	ComputeGrid(const JFloat xMin, const JFloat xMax, const JSize xCount,
								const JFloat yMin, const JFloat yMax, const JSize yCount);

	void		SetData(const JSize xCount, const JSize yCount, JMatrix* data);

	const JRGB&	GetMinColor() const;
	const JRGB&	GetMidColor() const;
	const JRGB&	GetMaxColor() const;
	void		SetColors(const JRGB& min, const JRGB& mid, const JRGB& max);

protected:

	virtual void	Render(J3DPainter& p);

private:

	JSize		itsXCount;
	JSize		itsYCount;
	JMatrix*	itsVertexList;
	JFloat		itsZMin;
	JFloat		itsZMax;
	JRGB		itsMinColor;
	JRGB		itsMidColor;
	JRGB		itsMaxColor;

private:

	void	PlaceVertex(J3DPainter& p, const JIndex rowIndex);

	// not allowed

	J3DSurface(const J3DSurface& source);
	const J3DSurface& operator=(const J3DSurface& source);
};


/******************************************************************************
 Color range

 ******************************************************************************/

inline const JRGB&
J3DSurface::GetMinColor()
	const
{
	return itsMinColor;
}

inline const JRGB&
J3DSurface::GetMidColor()
	const
{
	return itsMidColor;
}

inline const JRGB&
J3DSurface::GetMaxColor()
	const
{
	return itsMaxColor;
}

inline void
J3DSurface::SetColors
	(
	const JRGB& min,
	const JRGB& mid,
	const JRGB& max
	)
{
	itsMinColor = min;
	itsMidColor = mid;
	itsMaxColor = max;
	Refresh();
}

#endif
