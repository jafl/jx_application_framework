/******************************************************************************
 CBEiffelCompleter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEiffelCompleter
#define _H_CBEiffelCompleter

#include "CBStringCompleter.h"

class CBEiffelCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBEiffelCompleter();

protected:

	CBEiffelCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		MatchCase(const JString& source, JString* target) const override;

private:

	static CBEiffelCompleter*	itsSelf;

private:

	// not allowed

	CBEiffelCompleter(const CBEiffelCompleter& source);
	const CBEiffelCompleter& operator=(const CBEiffelCompleter& source);
};

#endif
