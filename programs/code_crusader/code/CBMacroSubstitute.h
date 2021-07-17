/******************************************************************************
 CBMacroSubstitute.h

	Copyright © 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBMacroSubstitute
#define _H_CBMacroSubstitute

#include <JSubstitute.h>

class CBMacroSubstitute : public JSubstitute
{
public:

	CBMacroSubstitute();

	virtual ~CBMacroSubstitute();

protected:

	virtual bool	Evaluate(JStringIterator& iter, JString* value) const override;

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
