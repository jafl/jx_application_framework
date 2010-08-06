/******************************************************************************
 JInterpolate.h

	Copyright © 1998 by Dustin Laurence.  All rights reserved.
	Copyright © 2005 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JInterpolate
#define _H_JInterpolate

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JSubstitute.h>
#include <JRegex.h>

class JInterpolate : public JSubstitute
{
public:

	JInterpolate();
	JInterpolate(const JCharacter* source, const pcre* regex,
				 const JArray<JIndexRange>* matchList);

	virtual ~JInterpolate();

	void	SetMatchResults(const JCharacter* source, const pcre* regex,
							const JArray<JIndexRange>* matchList);

protected:

	virtual JBoolean GetValue(const JString& name, JString* value) const;

private:

	const JCharacter*			itsSource;
	const pcre*					itsRegex;
	const JArray<JIndexRange>*	itsMatchList;

private:

	// not allowed

	JInterpolate(const JInterpolate& source);
	const JInterpolate& operator=(const JInterpolate& source);
};


/******************************************************************************
 SetMatchResults

 *****************************************************************************/

inline void
JInterpolate::SetMatchResults
	(
	const JCharacter*			source,
	const pcre*					regex,
	const JArray<JIndexRange>*	matchList
	)
{
	itsSource    = source;
	itsRegex     = regex;
	itsMatchList = matchList;
}

#endif
