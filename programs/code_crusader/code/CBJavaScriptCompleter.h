/******************************************************************************
 CBJavaScriptCompleter.h

	Copyright © 2006 by John Lindal.

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

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	CBJavaScriptCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		MatchCase(const JString& source, JString* target) const override;

private:

	static CBJavaScriptCompleter*	itsSelf;

private:

	// not allowed

	CBJavaScriptCompleter(const CBJavaScriptCompleter& source);
	const CBJavaScriptCompleter& operator=(const CBJavaScriptCompleter& source);
};

#endif
