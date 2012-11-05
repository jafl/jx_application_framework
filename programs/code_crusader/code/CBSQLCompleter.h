/******************************************************************************
 CBSQLCompleter.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSQLCompleter
#define _H_CBSQLCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBSQLCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBSQLCompleter();

protected:

	CBSQLCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBSQLCompleter*	itsSelf;

private:

	// not allowed

	CBSQLCompleter(const CBSQLCompleter& source);
	const CBSQLCompleter& operator=(const CBSQLCompleter& source);
};

#endif
