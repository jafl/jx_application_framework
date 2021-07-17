/******************************************************************************
 CBPerlCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPerlCompleter
#define _H_CBPerlCompleter

#include "CBStringCompleter.h"

class CBPerlCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBPerlCompleter();

protected:

	CBPerlCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBPerlCompleter*	itsSelf;

private:

	// not allowed

	CBPerlCompleter(const CBPerlCompleter& source);
	const CBPerlCompleter& operator=(const CBPerlCompleter& source);
};

#endif
