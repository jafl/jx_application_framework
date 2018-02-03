/******************************************************************************
 J3DComposite.h

	Interface for the J3DComposite class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_J3DComposite
#define _H_J3DComposite

#include <J3DObject.h>

class J3DComposite : public J3DObject
{
public:

	J3DComposite(J3DObject* parent, const JFloat x, const JFloat y, const JFloat z);
	J3DComposite(J3DObject* parent, const JVector& position);

	virtual ~J3DComposite();

protected:

	virtual void	Render(J3DPainter& p);

private:

	// not allowed

	J3DComposite(const J3DComposite& source);
	const J3DComposite& operator=(const J3DComposite& source);
};

#endif
