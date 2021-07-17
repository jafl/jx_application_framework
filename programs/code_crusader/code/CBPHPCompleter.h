/******************************************************************************
 CBPHPCompleter.h

	Copyright © 2000 by John Lindal.

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

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		UpdateWordList() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBPHPCompleter*	itsSelf;

private:

	// not allowed

	CBPHPCompleter(const CBPHPCompleter& source);
	const CBPHPCompleter& operator=(const CBPHPCompleter& source);
};

#endif
