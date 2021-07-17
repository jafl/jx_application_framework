/******************************************************************************
 CBDCompleter.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDCompleter
#define _H_CBDCompleter

#include "CBStringCompleter.h"

class CBDCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBDCompleter();

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	CBDCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBDCompleter*	itsSelf;

private:

	// not allowed

	CBDCompleter(const CBDCompleter& source);
	const CBDCompleter& operator=(const CBDCompleter& source);
};

#endif
