/******************************************************************************
 JGetCurrentColormap.h

	Interface for the JGetCurrentColormap class.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JGetCurrentColormap
#define _H_JGetCurrentColormap

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

class JColormap;

class JGetCurrentColormap
{
public:

	JGetCurrentColormap();

	virtual ~JGetCurrentColormap();

	virtual JColormap*	GetCurrColormap() const = 0;

private:

	// not allowed

	JGetCurrentColormap(const JGetCurrentColormap& source);
	const JGetCurrentColormap& operator=(const JGetCurrentColormap& source);
};

#endif
