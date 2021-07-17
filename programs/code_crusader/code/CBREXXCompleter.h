/******************************************************************************
 CBREXXCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBREXXCompleter
#define _H_CBREXXCompleter

#include "CBStringCompleter.h"

class CBREXXCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBREXXCompleter();

protected:

	CBREXXCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBREXXCompleter*	itsSelf;

private:

	// not allowed

	CBREXXCompleter(const CBREXXCompleter& source);
	const CBREXXCompleter& operator=(const CBREXXCompleter& source);
};

#endif
