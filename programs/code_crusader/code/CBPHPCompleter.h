/******************************************************************************
 CBPHPCompleter.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPHPCompleter
#define _H_CBPHPCompleter

#include "CBStringCompleter.h"

class CBPHPCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBPHPCompleter();

protected:

	CBPHPCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;
	virtual void		UpdateWordList();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	static CBPHPCompleter*	itsSelf;

private:

	// not allowed

	CBPHPCompleter(const CBPHPCompleter& source);
	const CBPHPCompleter& operator=(const CBPHPCompleter& source);
};

#endif
