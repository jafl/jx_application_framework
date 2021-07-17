/******************************************************************************
 CBJavaCompleter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBJavaCompleter
#define _H_CBJavaCompleter

#include "CBStringCompleter.h"

class CBJavaCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBJavaCompleter();

protected:

	CBJavaCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBJavaCompleter*	itsSelf;

private:

	// not allowed

	CBJavaCompleter(const CBJavaCompleter& source);
	const CBJavaCompleter& operator=(const CBJavaCompleter& source);
};

#endif
