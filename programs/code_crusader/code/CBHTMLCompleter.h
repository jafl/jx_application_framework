/******************************************************************************
 CBHTMLCompleter.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBHTMLCompleter
#define _H_CBHTMLCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBHTMLCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBHTMLCompleter();

	static JSize	GetDefaultWordList(const JCharacter*** list);

protected:

	CBHTMLCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBHTMLCompleter*	itsSelf;

private:

	// not allowed

	CBHTMLCompleter(const CBHTMLCompleter& source);
	const CBHTMLCompleter& operator=(const CBHTMLCompleter& source);
};

#endif
