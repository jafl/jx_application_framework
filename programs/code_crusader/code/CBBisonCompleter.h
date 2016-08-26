/******************************************************************************
 CBBisonCompleter.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBBisonCompleter
#define _H_CBBisonCompleter

#include "CBStringCompleter.h"

class CBBisonCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBBisonCompleter();

protected:

	CBBisonCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;
	virtual void		UpdateWordList();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	static CBBisonCompleter*	itsSelf;

private:

	// not allowed

	CBBisonCompleter(const CBBisonCompleter& source);
	const CBBisonCompleter& operator=(const CBBisonCompleter& source);
};

#endif
