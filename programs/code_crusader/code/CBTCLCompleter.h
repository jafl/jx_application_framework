/******************************************************************************
 CBTCLCompleter.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTCLCompleter
#define _H_CBTCLCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBTCLCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBTCLCompleter();

protected:

	CBTCLCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBTCLCompleter*	itsSelf;

private:

	// not allowed

	CBTCLCompleter(const CBTCLCompleter& source);
	const CBTCLCompleter& operator=(const CBTCLCompleter& source);
};

#endif
