/******************************************************************************
 CBSearchFontManager.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSearchFontManager
#define _H_CBSearchFontManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFontManager.h>

class CBSearchFontManager : public JFontManager
{
public:

	CBSearchFontManager();

	virtual ~CBSearchFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const;
	virtual JBoolean	GetFontSizes(const JCharacter* name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const;

protected:

	virtual JFontID				GetFontID(const JCharacter* name, const JSize size,
										  const JFontStyle& style) const;
	virtual const JCharacter*	GetFontName(const JFontID id) const;
	virtual JBoolean			IsExact(const JFontID id) const;

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const;

	virtual JSize	GetCharWidth(const JFontID fontID, const JCharacter c) const;

	virtual JSize	GetStringWidth(const JFontID fontID,
								   const JCharacter* str, const JSize charCount) const;

private:

	// not allowed

	CBSearchFontManager(const CBSearchFontManager& source);
	const CBSearchFontManager& operator=(const CBSearchFontManager& source);
};

#endif
