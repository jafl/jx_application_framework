/******************************************************************************
 J3DCone.h

	Interface for the J3DCone class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J3DCone
#define _H_J3DCone

#include <J3DCylinder.h>

class J3DCone : public J3DCylinder
{
public:

	J3DCone(J3DObject* parent, const JFloat x, const JFloat y, const JFloat z,
			const JFloat baseR, const JFloat height);
	J3DCone(J3DObject* parent, const JVector& position,
			const JFloat baseR, const JFloat height);

	virtual ~J3DCone();

private:

	// not allowed

	J3DCone(const J3DCone& source);
	const J3DCone& operator=(const J3DCone& source);
};

#endif
