/******************************************************************************
 JXXFontMenu.h

	Interface for the JXXFontMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXXFontMenu
#define _H_JXXFontMenu

#include "jx-af/jx/JXFontNameMenu.h"
#include "jx-af/jx/JXFontManager.h"		// need definition of JSortXFontNamesFn

class JXXFontMenu : public JXFontNameMenu
{
public:

	static bool Create(const JRegex& regex, JSortXFontNamesFn compare,
						   const JString& title, JXContainer* enclosure,
						   const HSizingOption hSizing, const VSizingOption vSizing,
						   const JCoordinate x, const JCoordinate y,
						   const JCoordinate w, const JCoordinate h,
						   JXXFontMenu** menu);

	static bool Create(const JRegex& regex, JSortXFontNamesFn compare,
						   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure,
						   JXXFontMenu** menu);

	virtual ~JXXFontMenu();

protected:

	JXXFontMenu(const JPtrArray<JString>& fontNames,
				const JString& title, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

private:

	JXXFontMenu(const JPtrArray<JString>& fontNames,
				JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	void	BuildMenu(const JPtrArray<JString>& fontNames);
};

#endif
