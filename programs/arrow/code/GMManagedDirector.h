/******************************************************************************
 GMManagedDirector.h

	Interface for the GMManagedDirector class

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMManagedDirector
#define _H_GMManagedDirector


#include <JXWindowDirector.h>

class JXImage;
class JString;

class GMManagedDirector : public JXWindowDirector
{
public:

	GMManagedDirector(JXDirector* supervisor);
	virtual ~GMManagedDirector();

	virtual JXImage*	GetMenuIcon() const = 0;
	virtual JBoolean	GetShortcut(JString* shortcut) const;
	virtual JBoolean	GetID(JString* id) const;

private:

	// not allowed

	GMManagedDirector(const GMManagedDirector& source);
	const GMManagedDirector& operator=(const GMManagedDirector& source);
};

#endif
