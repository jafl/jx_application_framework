/******************************************************************************
 JInterpolate.h

	Copyright (C) 1998 by Dustin Laurence.  All rights reserved.
	Copyright (C) 2005 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JInterpolate
#define _H_JInterpolate

#include <JSubstitute.h>
#include <JRegex.h>

class JInterpolate : public JSubstitute
{
public:

	JInterpolate();
	JInterpolate(const JUtf8Byte* source, const pcre* regex,
				 const JArray<JIndexRange>* matchList);

	virtual ~JInterpolate();

	void	SetMatchResults(const JUtf8Byte* source, const pcre* regex,
							const JArray<JIndexRange>* matchList);

protected:

	virtual JBoolean GetValue(const JString& name, JString* value) const;

private:

	const JUtf8Byte*			itsSource;
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
	const JUtf8Byte*			source,
	const pcre*					regex,
	const JArray<JIndexRange>*	matchList
	)
{
	itsSource    = source;
	itsRegex     = regex;
	itsMatchList = matchList;
}

#endif
