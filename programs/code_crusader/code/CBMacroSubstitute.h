/******************************************************************************
 CBMacroSubstitute.h

	Copyright © 1998 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBMacroSubstitute
#define _H_CBMacroSubstitute

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JSubstitute.h>

class CBMacroSubstitute : public JSubstitute
{
public:

	CBMacroSubstitute();

	virtual ~CBMacroSubstitute();

	static JBoolean	GetExecRange(const JString& s, const JIndex startIndex,
								 JIndexRange* matchRange);

protected:

	virtual JBoolean	Evaluate(const JString& s, const JIndex startIndex,
								 JIndexRange* matchRange, JString* value) const;

private:

	JSize	itsExecCount;

private:

	void	TurnOnEscapes();
	void	TurnOffEscapes();

	// not allowed

	CBMacroSubstitute(const CBMacroSubstitute& source);
	const CBMacroSubstitute& operator=(const CBMacroSubstitute& source);
};

#endif
