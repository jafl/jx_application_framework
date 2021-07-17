/******************************************************************************
 CBGoCompleter.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBGoCompleter
#define _H_CBGoCompleter

#include "CBStringCompleter.h"

class CBGoCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBGoCompleter();

protected:

	CBGoCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBGoCompleter*	itsSelf;

private:

	// not allowed

	CBGoCompleter(const CBGoCompleter& source);
	const CBGoCompleter& operator=(const CBGoCompleter& source);
};

#endif
