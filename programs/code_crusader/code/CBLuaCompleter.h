/******************************************************************************
 CBLuaCompleter.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBLuaCompleter
#define _H_CBLuaCompleter

#include "CBStringCompleter.h"

class CBLuaCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBLuaCompleter();

protected:

	CBLuaCompleter();

	virtual JBoolean	IsWordCharacter(const JUtf8Character& c,
										const JBoolean includeNS) const override;

private:

	static CBLuaCompleter*	itsSelf;

private:

	// not allowed

	CBLuaCompleter(const CBLuaCompleter& source);
	const CBLuaCompleter& operator=(const CBLuaCompleter& source);
};

#endif
