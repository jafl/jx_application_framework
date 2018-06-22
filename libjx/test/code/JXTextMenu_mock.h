/******************************************************************************
 JXTextMenu.h

	Mock for the JXTextMenu class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenu
#define _H_JXTextMenu

#include <JXMenu_mock.h>

class JXTextMenu : public JXMenu
{
public:

	JXTextMenu(JXDisplay* d, JFontManager* f);
};

#endif
