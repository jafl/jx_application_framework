/******************************************************************************
 CBCShellCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCShellCompleter
#define _H_CBCShellCompleter

#include "CBStringCompleter.h"

class CBCShellCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBCShellCompleter();

protected:

	CBCShellCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBCShellCompleter*	itsSelf;

private:

	// not allowed

	CBCShellCompleter(const CBCShellCompleter& source);
	const CBCShellCompleter& operator=(const CBCShellCompleter& source);
};

#endif
