/******************************************************************************
 CBCCompleter.h

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCCompleter
#define _H_CBCCompleter

#include "CBStringCompleter.h"

class CBCCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBCCompleter();

	static JSize	GetDefaultWordList(const JCharacter*** list);

protected:

	CBCCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBCCompleter*	itsSelf;

private:

	// not allowed

	CBCCompleter(const CBCCompleter& source);
	const CBCCompleter& operator=(const CBCCompleter& source);
};

#endif
