/******************************************************************************
 JFontManager.h

	Interface for the JFontManager class.

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFontManager
#define _H_JFontManager

#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JStringMap.h"
#include "jx-af/jcore/JFont.h"

class JFontManager
{
	friend class JFont;

public:

	static void	Init(const JUtf8Byte* defaultFontName,
					 const JUtf8Byte* defaultMonospaceFontName);

	virtual ~JFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) = 0;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) = 0;
	virtual bool	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) = 0;

	static const JString&	GetDefaultFontName();
	static const JString&	GetDefaultMonospaceFontName();

	static JSize	GetDefaultFontSize();
	static JSize	GetDefaultRowColHeaderFontSize();
	static JSize	GetDefaultMonospaceFontSize();

	static JFont	GetDefaultFont();
	static JFont	GetDefaultMonospaceFont();
	static JFont	GetFont(const JString& name,
							const JSize size = 0,
							const JFontStyle style = JFontStyle());

protected:

	JFontManager();

	static JFontID	GetFontID(const JString& name, const JSize size,
							  const JFontStyle& style);
	static JFont	GetFont(const JFontID id);

	static const JString&	GetFontName(const JFontID id);

	static JSize	GetStrikeThickness(const JSize fontSize);
	static JSize	GetUnderlineThickness(const JSize fontSize);

	virtual JSize	GetLineHeight(const JFontID id, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) = 0;

	virtual JSize	GetCharWidth(const JFontID id, const JUtf8Character& c) = 0;
	virtual JSize	GetStringWidth(const JFontID id, const JString& str) = 0;

	virtual bool	IsExact(const JFontID id) = 0;
	virtual bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c) = 0;
	virtual bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name) = 0;

public:		// ought to be private

	struct Font
	{
		JString*	name;
		JSize		size;
		JFontStyle	style;
	};

private:

	static JString		theDefaultFontName;
	static JSize		theDefaultFontSize;
	static JSize		theDefaultRCHFontSize;

	static JString		theDefaultMonospaceFontName;
	static JSize		theDefaultMonospaceFontSize;

	static JArray<Font>	theFontList;
	static JFontID		theDefaultFontID;
	static JFontID		theDefaultMonospaceFontID;

private:

	static void	CleanUp();

	// not allowed

	JFontManager(const JFontManager&) = delete;
	JFontManager& operator=(const JFontManager&) = delete;
};


/******************************************************************************
 Get defaults (static)

 ******************************************************************************/

inline const JString&
JFontManager::GetDefaultFontName()
{
	return theDefaultFontName;
}

inline JSize
JFontManager::GetDefaultFontSize()
{
	return theDefaultFontSize;
}

inline JSize
JFontManager::GetDefaultRowColHeaderFontSize()
{
	return theDefaultRCHFontSize;
}

inline const JString&
JFontManager::GetDefaultMonospaceFontName()
{
	return theDefaultMonospaceFontName;
}

inline JSize
JFontManager::GetDefaultMonospaceFontSize()
{
	return theDefaultMonospaceFontSize;
}

/******************************************************************************
 Get line thicknesses (static protected)

 ******************************************************************************/

inline JSize
JFontManager::GetStrikeThickness
	(
	const JSize fontSize
	)
{
	return (fontSize>=24 ? fontSize/12 : 1);
}

inline JSize
JFontManager::GetUnderlineThickness
	(
	const JSize fontSize
	)
{
	return (1 + fontSize/32);
}

#endif
