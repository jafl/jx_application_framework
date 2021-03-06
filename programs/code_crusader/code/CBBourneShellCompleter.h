/******************************************************************************
 CBBourneShellCompleter.h

	Copyright (C) 2000 by John Lindal. All rights reserved.

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

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBBourneShellCompleter*	itsSelf;

private:

	// not allowed

	CBBourneShellCompleter(const CBBourneShellCompleter& source);
	const CBBourneShellCompleter& operator=(const CBBourneShellCompleter& source);
};

#endif
