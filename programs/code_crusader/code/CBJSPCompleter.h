/******************************************************************************
 CBJSPCompleter.h

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBJSPCompleter
#define _H_CBJSPCompleter

#include "CBStringCompleter.h"

class CBJSPCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBJSPCompleter();

protected:

	CBJSPCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;
	virtual void		UpdateWordList();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	static CBJSPCompleter*	itsSelf;

private:

	// not allowed

	CBJSPCompleter(const CBJSPCompleter& source);
	const CBJSPCompleter& operator=(const CBJSPCompleter& source);
};

#endif
