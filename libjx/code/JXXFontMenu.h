/******************************************************************************
 JXXFontMenu.h

	Interface for the JXXFontMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXXFontMenu
#define _H_JXXFontMenu

#include <JXFontNameMenu.h>
#include <JXFontManager.h>		// need definition of JSortXFontNamesFn

class JXXFontMenu : public JXFontNameMenu
{
public:

	static JBoolean Create(const JRegex& regex, JSortXFontNamesFn compare,
						   const JCharacter* title, JXContainer* enclosure,
						   const HSizingOption hSizing, const VSizingOption vSizing,
						   const JCoordinate x, const JCoordinate y,
						   const JCoordinate w, const JCoordinate h,
						   JXXFontMenu** menu);

	static JBoolean Create(const JRegex& regex, JSortXFontNamesFn compare,
						   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure,
						   JXXFontMenu** menu);

	virtual ~JXXFontMenu();

protected:

	JXXFontMenu(const JPtrArray<JString>& fontNames,
				const JCharacter* title, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

private:

	JXXFontMenu(const JPtrArray<JString>& fontNames,
				JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	void	BuildMenu(const JPtrArray<JString>& fontNames);

	// not allowed

	JXXFontMenu(const JXXFontMenu& source);
	const JXXFontMenu& operator=(const JXXFontMenu& source);
};

#endif
