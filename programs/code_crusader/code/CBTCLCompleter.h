/******************************************************************************
 CBTCLCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTCLCompleter
#define _H_CBTCLCompleter

#include "CBStringCompleter.h"

class CBTCLCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBTCLCompleter();

protected:

	CBTCLCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBTCLCompleter*	itsSelf;

private:

	// not allowed

	CBTCLCompleter(const CBTCLCompleter& source);
	const CBTCLCompleter& operator=(const CBTCLCompleter& source);
};

#endif
