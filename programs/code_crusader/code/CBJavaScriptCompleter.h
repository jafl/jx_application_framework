/******************************************************************************
 CBJavaScriptCompleter.h

	Copyright © 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBJavaScriptCompleter
#define _H_CBJavaScriptCompleter

#include "CBStringCompleter.h"

class CBJavaScriptCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBJavaScriptCompleter();

	static JSize	GetDefaultWordList(const JCharacter*** list);

protected:

	CBJavaScriptCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;
	virtual void		MatchCase(const JString& source, JString* target) const;

private:

	static CBJavaScriptCompleter*	itsSelf;

private:

	// not allowed

	CBJavaScriptCompleter(const CBJavaScriptCompleter& source);
	const CBJavaScriptCompleter& operator=(const CBJavaScriptCompleter& source);
};

#endif
