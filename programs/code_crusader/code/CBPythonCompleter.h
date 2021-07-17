/******************************************************************************
 CBPythonCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPythonCompleter
#define _H_CBPythonCompleter

#include "CBStringCompleter.h"

class CBPythonCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBPythonCompleter();

protected:

	CBPythonCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBPythonCompleter*	itsSelf;

private:

	// not allowed

	CBPythonCompleter(const CBPythonCompleter& source);
	const CBPythonCompleter& operator=(const CBPythonCompleter& source);
};

#endif
