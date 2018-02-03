/******************************************************************************
 CBPascalCompleter.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPascalCompleter
#define _H_CBPascalCompleter

#include "CBStringCompleter.h"

class CBPascalCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBPascalCompleter();

protected:

	CBPascalCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBPascalCompleter*	itsSelf;

private:

	// not allowed

	CBPascalCompleter(const CBPascalCompleter& source);
	const CBPascalCompleter& operator=(const CBPascalCompleter& source);
};

#endif
