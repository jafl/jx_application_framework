/******************************************************************************
 CBBisonCompleter.h

	Copyright © 2001 by John Lindal.

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

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		UpdateWordList() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBBisonCompleter*	itsSelf;

private:

	// not allowed

	CBBisonCompleter(const CBBisonCompleter& source);
	const CBBisonCompleter& operator=(const CBBisonCompleter& source);
};

#endif
