/******************************************************************************
 CBLexCompleter.h

	Copyright © 2001 by John Lindal. All rights reserved.

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

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;
	virtual void		UpdateWordList();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	static CBLexCompleter*	itsSelf;

private:

	// not allowed

	CBLexCompleter(const CBLexCompleter& source);
	const CBLexCompleter& operator=(const CBLexCompleter& source);
};

#endif
