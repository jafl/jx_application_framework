/******************************************************************************
 J3DUniverse.h

	Interface for the J3DUniverse class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J3DUniverse
#define _H_J3DUniverse

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <J3DObject.h>

class J3DCamera;

class J3DUniverse : public J3DObject
{
public:

	J3DUniverse(const JColormap& colormap);

	virtual ~J3DUniverse();

	const JColormap&	GetColormap() const;

	// called by J3DCamera

	void	AddCamera(J3DCamera* camera);
	void	RemoveCamera(J3DCamera* camera);

protected:

	virtual void	Render(J3DPainter& p);

	virtual void	Refresh();
	virtual void	Redraw();

	virtual JColorIndex	GetDefaultColor() const;

private:

	JPtrArray<J3DCamera>*	itsCameraList;	// must store them so we can refresh them
	const JColormap&		itsColormap;	// provides default color

private:

	// not allowed

	J3DUniverse(const J3DUniverse& source);
	const J3DUniverse& operator=(const J3DUniverse& source);
};


/******************************************************************************
 GetColormap

 ******************************************************************************/

inline const JColormap&
J3DUniverse::GetColormap()
	const
{
	return itsColormap;
}

#endif
