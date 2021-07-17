/******************************************************************************
 CBRubyCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBRubyCompleter
#define _H_CBRubyCompleter

#include "CBStringCompleter.h"

class CBRubyCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBRubyCompleter();

protected:

	CBRubyCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBRubyCompleter*	itsSelf;

private:

	// not allowed

	CBRubyCompleter(const CBRubyCompleter& source);
	const CBRubyCompleter& operator=(const CBRubyCompleter& source);
};

#endif
