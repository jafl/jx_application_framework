/******************************************************************************
 CBRubyCompleter.h

	Copyright (C) 2001 by John Lindal.

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

	virtual JBoolean	IsWordCharacter(const JUtf8Character& c,
										const JBoolean includeNS) const override;

private:

	static CBRubyCompleter*	itsSelf;

private:

	// not allowed

	CBRubyCompleter(const CBRubyCompleter& source);
	const CBRubyCompleter& operator=(const CBRubyCompleter& source);
};

#endif
