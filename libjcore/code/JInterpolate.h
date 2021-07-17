/******************************************************************************
 JInterpolate.h

	Copyright (C) 1998 by Dustin Laurence.
	Copyright (C) 2005-17 by John Lindal.

 *****************************************************************************/

#ifndef _H_JInterpolate
#define _H_JInterpolate

#include "JSubstitute.h"

class JStringMatch;

class JInterpolate : public JSubstitute
{
public:

	JInterpolate();

	virtual ~JInterpolate();

	JString	Interpolate(const JString& pattern, const JStringMatch& m);

protected:

	virtual bool GetValue(const JString& name, JString* value) const;

private:

	const JStringMatch* itsMatch;

private:

	// not allowed

	JInterpolate(const JInterpolate& source);
	const JInterpolate& operator=(const JInterpolate& source);
};

#endif
