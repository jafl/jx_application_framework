/******************************************************************************
 CBCSharpCompleter.h

	Copyright (C) 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCSharpCompleter
#define _H_CBCSharpCompleter

#include "CBStringCompleter.h"

class CBCSharpCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBCSharpCompleter();

protected:

	CBCSharpCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBCSharpCompleter*	itsSelf;

private:

	// not allowed

	CBCSharpCompleter(const CBCSharpCompleter& source);
	const CBCSharpCompleter& operator=(const CBCSharpCompleter& source);
};

#endif
