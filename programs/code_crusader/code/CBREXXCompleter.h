/******************************************************************************
 CBREXXCompleter.h

	Copyright © 2001 by John Lindal. All rights reserved.

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

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBREXXCompleter*	itsSelf;

private:

	// not allowed

	CBREXXCompleter(const CBREXXCompleter& source);
	const CBREXXCompleter& operator=(const CBREXXCompleter& source);
};

#endif
