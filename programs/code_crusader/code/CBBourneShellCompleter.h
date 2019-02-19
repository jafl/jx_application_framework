/******************************************************************************
 CBBourneShellCompleter.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBBourneShellCompleter
#define _H_CBBourneShellCompleter

#include "CBStringCompleter.h"

class CBBourneShellCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBBourneShellCompleter();

protected:

	CBBourneShellCompleter();

	virtual JBoolean	IsWordCharacter(const JUtf8Character& c,
										const JBoolean includeNS) const override;

private:

	static CBBourneShellCompleter*	itsSelf;

private:

	// not allowed

	CBBourneShellCompleter(const CBBourneShellCompleter& source);
	const CBBourneShellCompleter& operator=(const CBBourneShellCompleter& source);
};

#endif
