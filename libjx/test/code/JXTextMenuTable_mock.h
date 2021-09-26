/******************************************************************************
 JXTextMenuTable.h

	Mock for the JXTextMenuTable class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenuTable
#define _H_JXTextMenuTable

#include <jx-af/jcore/jTypes.h>

class JString;
class JFont;
class JFontManager;
class JXDisplay;

class JXTextMenuTable
{
public:

	static void		AdjustFont(JXDisplay* display, const JIndex colIndex,
							   const JString& text, JFont* font);
	static JSize	GetTextWidth(JFontManager* fontMgr, const JFont& font,
								 const JIndex colIndex, const JString& text);
};

#endif
