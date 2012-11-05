/******************************************************************************
 CBMakeCompleter.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBMakeCompleter
#define _H_CBMakeCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBMakeCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBMakeCompleter();

protected:

	CBMakeCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBMakeCompleter*	itsSelf;

private:

	// not allowed

	CBMakeCompleter(const CBMakeCompleter& source);
	const CBMakeCompleter& operator=(const CBMakeCompleter& source);
};

#endif
