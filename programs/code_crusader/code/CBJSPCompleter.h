/******************************************************************************
 CBJSPCompleter.h

	Copyright © 2007 by John Lindal.

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

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		UpdateWordList() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBJSPCompleter*	itsSelf;

private:

	// not allowed

	CBJSPCompleter(const CBJSPCompleter& source);
	const CBJSPCompleter& operator=(const CBJSPCompleter& source);
};

#endif
