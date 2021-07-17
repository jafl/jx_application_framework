/******************************************************************************
 CBLexCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBLexCompleter
#define _H_CBLexCompleter

#include "CBStringCompleter.h"

class CBLexCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBLexCompleter();

protected:

	CBLexCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		UpdateWordList() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBLexCompleter*	itsSelf;

private:

	// not allowed

	CBLexCompleter(const CBLexCompleter& source);
	const CBLexCompleter& operator=(const CBLexCompleter& source);
};

#endif
