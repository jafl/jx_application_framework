/******************************************************************************
 CBSQLCompleter.h

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSQLCompleter
#define _H_CBSQLCompleter

#include "CBStringCompleter.h"

class CBSQLCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBSQLCompleter();

protected:

	CBSQLCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		MatchCase(const JString& source, JString* target) const override;

private:

	static CBSQLCompleter*	itsSelf;

private:

	// not allowed

	CBSQLCompleter(const CBSQLCompleter& source);
	const CBSQLCompleter& operator=(const CBSQLCompleter& source);
};

#endif
