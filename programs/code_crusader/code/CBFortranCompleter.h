/******************************************************************************
 CBFortranCompleter.h

	Copyright © 2000 by John Lindal. All rights reserved.

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

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;
	virtual void		MatchCase(const JString& source, JString* target) const;

private:

	static CBFortranCompleter*	itsSelf;

private:

	// not allowed

	CBFortranCompleter(const CBFortranCompleter& source);
	const CBFortranCompleter& operator=(const CBFortranCompleter& source);
};

#endif
