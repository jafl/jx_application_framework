/******************************************************************************
 CBCCompleter.h

	Copyright (C) 1998 by John Lindal.

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

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	CBCCompleter();

	virtual JBoolean	IsWordCharacter(const JUtf8Character& c,
										const JBoolean includeNS) const override;

private:

	static CBCCompleter*	itsSelf;

private:

	// not allowed

	CBCCompleter(const CBCCompleter& source);
	const CBCCompleter& operator=(const CBCCompleter& source);
};

#endif
