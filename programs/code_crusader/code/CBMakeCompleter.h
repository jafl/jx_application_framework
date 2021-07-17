/******************************************************************************
 CBMakeCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBMakeCompleter
#define _H_CBMakeCompleter

#include "CBStringCompleter.h"

class CBMakeCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBMakeCompleter();

protected:

	CBMakeCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBMakeCompleter*	itsSelf;

private:

	// not allowed

	CBMakeCompleter(const CBMakeCompleter& source);
	const CBMakeCompleter& operator=(const CBMakeCompleter& source);
};

#endif
