/******************************************************************************
 J3DCamera.h

	Interface for the J3DCamera class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_J3DCamera
#define _H_J3DCamera

#include <JVector.h>

class J3DUniverse;

class J3DCamera
{
public:

	J3DCamera(J3DUniverse* universe);
	J3DCamera(J3DUniverse* universe, const JVector& position,
			  const JVector& attentionPt, const JVector& upVector);

	virtual ~J3DCamera();

	virtual void	Render() = 0;
	virtual void	Refresh() = 0;
	virtual void	Redraw() = 0;
	virtual void	UpdateViewport() = 0;

	const JVector&	GetPosition() const;
	void			SetPosition(const JVector& position);
	void			AdjustPosition(const JVector& delta);

	const JVector&	GetAttentionPt() const;
	void			SetAttentionPt(const JVector& pt);
	void			AdjustAttentionPt(const JVector& delta);

	const JVector&	GetUpVector() const;
	void			SetUpVector(const JVector& v);
	void			AdjustUpVector(const JVector& delta);

	J3DUniverse*	GetUniverse() const;

	virtual JVector	ModelToScreen(const JVector& pt) = 0;
	virtual JVector	ScreenToModel(const JVector& pt) = 0;

private:

	J3DUniverse*	itsUniverse;	// not owned

	JVector	itsPosition;
	JVector	itsAttentionPt;
	JVector	itsUpVector;

private:

	// not allowed

	J3DCamera(const J3DCamera& source);
	const J3DCamera& operator=(const J3DCamera& source);
};


/******************************************************************************
 Position

 ******************************************************************************/

inline const JVector&
J3DCamera::GetPosition()
	const
{
	return itsPosition;
}

inline void
J3DCamera::SetPosition
	(
	const JVector& position
	)
{
	itsPosition = position;
	Refresh();
}

inline void
J3DCamera::AdjustPosition
	(
	const JVector& delta
	)
{
	itsPosition += delta;
	Refresh();
}

/******************************************************************************
 Attention point

 ******************************************************************************/

inline const JVector&
J3DCamera::GetAttentionPt()
	const
{
	return itsAttentionPt;
}

inline void
J3DCamera::SetAttentionPt
	(
	const JVector& pt
	)
{
	itsAttentionPt = pt;
	Refresh();
}

inline void
J3DCamera::AdjustAttentionPt
	(
	const JVector& delta
	)
{
	itsAttentionPt += delta;
	Refresh();
}

/******************************************************************************
 Up vector

 ******************************************************************************/

inline const JVector&
J3DCamera::GetUpVector()
	const
{
	return itsUpVector;
}

inline void
J3DCamera::SetUpVector
	(
	const JVector& v
	)
{
	itsUpVector = v;
	Refresh();
}

inline void
J3DCamera::AdjustUpVector
	(
	const JVector& delta
	)
{
	itsUpVector += delta;
	Refresh();
}

/******************************************************************************
 GetUniverse

 ******************************************************************************/

inline J3DUniverse*
J3DCamera::GetUniverse()
	const
{
	return itsUniverse;
}

#endif
