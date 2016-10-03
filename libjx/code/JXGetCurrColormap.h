/******************************************************************************
 JXGetCurrColormap.h

	Interface for the JXGetCurrColormap class.

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetCurrColormap
#define _H_JXGetCurrColormap

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
