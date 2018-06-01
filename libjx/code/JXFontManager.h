/******************************************************************************
 JXFontManager.h

	Interface for the JXFontManager class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontManager
#define _H_JXFontManager

#include <JFontManager.h>
#include <JArray.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

class JString;
class JRegex;
class JXDisplay;

typedef JListT::CompareResult
	(*JSortXFontNamesFn)(JString * const &, JString * const &);

class JXFontManager : public JFontManager
{
	friend class JXGC;

public:

	enum FontType
	{
		kStdType,
		kTrueType
	};

	struct XFont
	{
		FontType	type;
		XFontSet	xfset;
		XftFont*	xftt;

		XFont()
			:
			type(kStdType), xfset(NULL), xftt(NULL)
		{ };

		void Free(JXDisplay* display);
	};

public:

	JXFontManager(JXDisplay* display);

	virtual ~JXFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames);
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames);
	virtual JBoolean	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList);

	virtual JBoolean	IsExact(const JFontID id);

	// for X Window System only

	void		GetXFontNames(const JRegex& regex,
							  JPtrArray<JString>* fontNames,
							  JSortXFontNamesFn compare = NULL);
	JBoolean	GetXFont(const JString& xFontStr, JFont** font);
	XFont		GetXFontInfo(const JFontID id);

protected:

	virtual JSize	GetLineHeight(const JFontID id, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent);

	virtual JSize	GetCharWidth(const JFontID id, const JUtf8Character& c);
	virtual JSize	GetStringWidth(const JFontID id, const JString& str);

private:

	struct FontInfo
	{
		JBoolean	filled;
		XFont		xfont;
		JBoolean	exact;		// kJTrue => exact match to requested specs
		JCoordinate	ascent;		// 0 until first computed
		JCoordinate	descent;	// 0 until first computed
		JCoordinate	monoWidth;	// 0 if not monospace

		FontInfo()
			:
			filled(kJFalse), exact(kJFalse), ascent(0), descent(0), monoWidth(0)
		{ };
	};

private:

	JXDisplay*			itsDisplay;
	JArray<FontInfo>*	itsFontList;

	JPtrArray<JString>*	itsAllFontNames;	// can be NULL
	JPtrArray<JString>*	itsMonoFontNames;	// can be NULL

private:

	FontInfo	ResolveFontID(const JFontID id);

	static JString	BuildStdFontName(const JString& xName, const JSize pointSize,
									 const JSize pixelSize, const JSize pixelWidth,
									 const JUtf8Byte* spacing,
									 const JFontStyle& style, const JUtf8Byte* italicStr);
	static JBoolean	BuildTrueTypeFontName(const JString& xName,
										  const JSize size, const JFontStyle& style,
										  JString* xFontStr);

	JBoolean	GetNewFont(const JString& name, const JSize size,
						   const JFontStyle& style, XFont* xfont);
	void		ApproximateFont(const JString& name, const JSize size,
								const JFontStyle& style, XFont* xfont);

	static JString		ConvertToXFontName(const JString& name);
	static void		ConvertToPSFontName(JString* name);

	JCoordinate		IsMonospace(const XFont& xfont);
	static JBoolean	IsPostscript(const JString& name);
	static JBoolean	IsUseless(const JString& name);

	// not allowed

	JXFontManager(const JXFontManager& source);
	const JXFontManager& operator=(const JXFontManager& source);
};


/******************************************************************************
 GetXFontInfo

 ******************************************************************************/

inline JXFontManager::XFont
JXFontManager::GetXFontInfo
	(
	const JFontID id
	)
{
	return ResolveFontID(id).xfont;
}

#endif
