/******************************************************************************
 CBFortranCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFortranCompleter
#define _H_CBFortranCompleter

#include "CBStringCompleter.h"

class CBFortranCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBFortranCompleter();

protected:

	CBFortranCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		MatchCase(const JString& source, JString* target) const override;

private:

	static CBFortranCompleter*	itsSelf;

private:

	// not allowed

	CBFortranCompleter(const CBFortranCompleter& source);
	const CBFortranCompleter& operator=(const CBFortranCompleter& source);
};

#endif
