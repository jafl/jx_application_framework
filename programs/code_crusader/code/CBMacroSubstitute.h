/******************************************************************************
 CBMacroSubstitute.h

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBMacroSubstitute
#define _H_CBMacroSubstitute

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
