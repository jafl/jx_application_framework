/******************************************************************************
 CBLuaCompleter.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBLuaCompleter
#define _H_CBLuaCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBLuaCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBLuaCompleter();

protected:

	CBLuaCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBLuaCompleter*	itsSelf;

private:

	// not allowed

	CBLuaCompleter(const CBLuaCompleter& source);
	const CBLuaCompleter& operator=(const CBLuaCompleter& source);
};

#endif
