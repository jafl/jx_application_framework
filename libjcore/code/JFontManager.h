/******************************************************************************
 JFontManager.h

	Interface for the JFontManager class.

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFontManager
#define _H_JFontManager

#include <JPtrArray-JString.h>
#include <JFont.h>

const JFontID kInvalidFontID = 0;

class JFontManager
{
	friend class JFont;

public:

	JFontManager();

	virtual ~JFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) = 0;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) = 0;
	virtual JBoolean	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) = 0;

	static JFont	GetDefaultFont();
	static JFont	GetDefaultMonospaceFont();
	static JFont	GetFont(const JString& name,
							const JSize size = 0,
							const JFontStyle style = JFontStyle());

	virtual JBoolean	IsExact(const JFontID id) = 0;

protected:

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

public:		// ought to be private

	struct Font
	{
		JString*	name;
		JSize		size;
		JFontStyle	style;
	};

private:

	static JArray<Font>	theFontList;
	static JFontID		theDefaultFontID;
	static JFontID		theDefaultMonospaceFontID;

private:

	// not allowed

	JFontManager(const JFontManager& source);
	const JFontManager& operator=(const JFontManager& source);
};


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
