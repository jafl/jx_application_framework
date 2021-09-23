/******************************************************************************
 JXFontNameMenuTable.h

	Interface for the JXFontNameMenuTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontNameMenuTable
#define _H_JXFontNameMenuTable

#include "JXTextMenuTable.h"

class JXFontNameMenu;

class JXFontNameMenuTable : public JXTextMenuTable
{
public:

	JXFontNameMenuTable(JXFontNameMenu* menu, JXTextMenuData* data, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JXFontNameMenuTable();

	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;

private:

	JXFontNameMenu*	itsMenu;	// it owns us
};

#endif
