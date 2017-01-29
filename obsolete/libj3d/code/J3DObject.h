/******************************************************************************
 J3DObject.h

	Interface for the J3DObject class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J3DObject
#define _H_J3DObject

#include <JPtrArray.h>
#include <j3DConstants.h>	// for convenience
#include <J3DPainter.h>		// for convenience

class J3DObject
{
public:

	J3DObject(J3DObject* parent, const JFloat x, const JFloat y, const JFloat z);
	J3DObject(J3DObject* parent, const JVector& position);

	virtual ~J3DObject();

	const JVector&	GetPosition() const;
	void			SetPosition(const JFloat x, const JFloat y, const JFloat z);
	void			SetPosition(const JVector& position);
	void			AdjustPosition(const JFloat xDelta,
								   const JFloat yDelta,
								   const JFloat zDelta);
	void			AdjustPosition(const JVector& delta);

	void	SetRotation(const JVector& zAxis, const JFloat longitude);

	const JVector&	GetScale() const;
	void			SetScale(const JFloat xScale,
							 const JFloat yScale,
							 const JFloat zScale);
	void			SetScale(const JVector& scale);

	J3DObject*	GetParent() const;
	void		SetParent(J3DObject* parent);

	JBoolean	HasChildren() const;
	JBoolean	GetChildIterator(JListIterator<J3DObject*>** iter) const;

	// called by derived class of J3DCamera

	void	RenderAll(J3DPainter& p);

protected:

	virtual void	Place();
	virtual void	Rotate();
	virtual void	Scale();
	virtual void	Render(J3DPainter& p) = 0;

	virtual void	Refresh();
	virtual void	Redraw();

	virtual JColorIndex	GetDefaultColor() const;

private:

	JVector	itsPosition;

	JFloat	itsZAngle;
	JVector	itsZRotator;
	JFloat	itsXYAngle;

	JVector	itsScale;

	J3DObject*				itsParent;			// owns us
	JPtrArray<J3DObject>*	itsChildList;
	JBoolean				itsIgnoreRemoveChildFlag;

private:

	void	J3DObjectX();

	void	AddChild(J3DObject* child);
	void	RemoveChild(J3DObject* child);

	// not allowed

	J3DObject(const J3DObject& source);
	const J3DObject& operator=(const J3DObject& source);
};


/******************************************************************************
 Position

 ******************************************************************************/

inline const JVector&
J3DObject::GetPosition()
	const
{
	return itsPosition;
}

inline void
J3DObject::SetPosition
	(
	const JFloat x,
	const JFloat y,
	const JFloat z
	)
{
	itsPosition.SetElement(1, x);
	itsPosition.SetElement(2, y);
	itsPosition.SetElement(3, z);
	Refresh();
}

inline void
J3DObject::SetPosition
	(
	const JVector& position
	)
{
	itsPosition = position;
	Refresh();
}

inline void
J3DObject::AdjustPosition
	(
	const JFloat xDelta,
	const JFloat yDelta,
	const JFloat zDelta
	)
{
	itsPosition.SetElement(1, itsPosition.GetElement(1) + xDelta);
	itsPosition.SetElement(2, itsPosition.GetElement(2) + yDelta);
	itsPosition.SetElement(3, itsPosition.GetElement(3) + zDelta);
	Refresh();
}

inline void
J3DObject::AdjustPosition
	(
	const JVector& delta
	)
{
	itsPosition += delta;
	Refresh();
}

/******************************************************************************
 Scale

 ******************************************************************************/

inline const JVector&
J3DObject::GetScale()
	const
{
	return itsScale;
}

inline void
J3DObject::SetScale
	(
	const JFloat xScale,
	const JFloat yScale,
	const JFloat zScale
	)
{
	itsScale.SetElement(1, xScale);
	itsScale.SetElement(2, yScale);
	itsScale.SetElement(3, zScale);
	Refresh();
}

inline void
J3DObject::SetScale
	(
	const JVector& scale
	)
{
	itsScale = scale;
	Refresh();
}

/******************************************************************************
 GetParent

 ******************************************************************************/

inline J3DObject*
J3DObject::GetParent()
	const
{
	return itsParent;
}

/******************************************************************************
 HasChildren

 ******************************************************************************/

inline JBoolean
J3DObject::HasChildren()
	const
{
	return JConvertToBoolean( itsChildList != NULL );
}

#endif
