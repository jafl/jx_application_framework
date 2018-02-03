/******************************************************************************
 J3DAxes.h

	Interface for the J3DAxes class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_J3DAxes
#define _H_J3DAxes

#include <J3DComposite.h>

class J3DAxes : public J3DComposite
{
public:

	J3DAxes(const JColormap& colormap, J3DObject* parent,
			const JFloat scale = 1.0);

	virtual ~J3DAxes();

private:

	// not allowed

	J3DAxes(const J3DAxes& source);
	const J3DAxes& operator=(const J3DAxes& source);
};

#endif
