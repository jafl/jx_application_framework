/******************************************************************************
 JXGetCurrColormap.h

	Interface for the JXGetCurrColormap class.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetCurrColormap
#define _H_JXGetCurrColormap

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JGetCurrentColormap.h>

class JXGetCurrColormap : public JGetCurrentColormap
{
public:

	JXGetCurrColormap();

	virtual ~JXGetCurrColormap();

	virtual JColormap*	GetCurrColormap() const;

private:

	// not allowed

	JXGetCurrColormap(const JXGetCurrColormap& source);
	const JXGetCurrColormap& operator=(const JXGetCurrColormap& source);
};

#endif
