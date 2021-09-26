/******************************************************************************
 JXContainer.h

	Mock for the JXContainer class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXContainer
#define _H_JXContainer

#include <jx-af/jx/JXKeyModifiers.h>

class JFontManager;
class JXDisplay;

class JXContainer
{
public:

	JXContainer(JXDisplay* d, JFontManager* f);

	JXDisplay*		GetDisplay() const;
	JFontManager*	GetFontManager() const;

private:

	JXDisplay*		itsDisplay;
	JFontManager*	itsFontManager;
};

#endif
