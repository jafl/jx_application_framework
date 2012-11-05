/******************************************************************************
 CBRubyCompleter.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBRubyCompleter
#define _H_CBRubyCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBRubyCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBRubyCompleter();

protected:

	CBRubyCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBRubyCompleter*	itsSelf;

private:

	// not allowed

	CBRubyCompleter(const CBRubyCompleter& source);
	const CBRubyCompleter& operator=(const CBRubyCompleter& source);
};

#endif
