/******************************************************************************
 J3DCylinder.h

	Interface for the J3DCylinder class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J3DCylinder
#define _H_J3DCylinder

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <J3DObject.h>
#include <GL/glu.h>

class J3DCylinder : public J3DObject
{
public:

	J3DCylinder(J3DObject* parent, const JFloat x, const JFloat y, const JFloat z,
				const JFloat baseR, const JFloat topR, const JFloat height);
	J3DCylinder(J3DObject* parent, const JVector& position,
				const JFloat baseR, const JFloat topR, const JFloat height);

	virtual ~J3DCylinder();

	JColorIndex	GetColor() const;
	void		SetColor(const JColorIndex color);

protected:

	virtual void	Render(J3DPainter& p);

private:

	JFloat	itsBaseRadius;
	JFloat	itsTopRadius;
	JFloat	itsHeight;

	JColorIndex		itsColor;

	GLUquadricObj*	itsQuadric;

private:

	void	J3DCylinderX(const JFloat baseR, const JFloat topR, const JFloat height);

	// not allowed

	J3DCylinder(const J3DCylinder& source);
	const J3DCylinder& operator=(const J3DCylinder& source);
};


/******************************************************************************
 Color

 ******************************************************************************/

inline JColorIndex
J3DCylinder::GetColor()
	const
{
	return itsColor;
}

inline void
J3DCylinder::SetColor
	(
	const JColorIndex color
	)
{
	itsColor = color;
	Refresh();
}

#endif
