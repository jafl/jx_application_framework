/******************************************************************************
 JXFontNameMenuDirector.h

	Interface for the JXFontNameMenuDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontNameMenuDirector
#define _H_JXFontNameMenuDirector

#include "JXTextMenuDirector.h"

class JXFontNameMenu;

class JXFontNameMenuDirector : public JXTextMenuDirector
{
public:

	JXFontNameMenuDirector(JXDirector* supervisor,
						   JXFontNameMenu* menu, JXTextMenuData* menuData);

	~JXFontNameMenuDirector() override;

protected:

	JXMenuTable*	CreateMenuTable() override;

private:

	JXFontNameMenu*	itsMenu;	// it owns us
};

#endif
