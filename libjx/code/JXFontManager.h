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
		FontType		type;
		XFontStruct*	xfstd;
		XftFont*		xftt;

		XFont()
			:
			type(kStdType), xfstd(NULL), xftt(NULL)
		{ };

		void Free(JXDisplay* display);
	};

public:

	JXFontManager(JXDisplay* display);

	virtual ~JXFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const;
	virtual JBoolean	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const;

	// for X Window System only

	void		GetXFontNames(const JRegex& regex,
							  JPtrArray<JString>* fontNames,
							  JSortXFontNamesFn compare = NULL) const;
	JBoolean	GetFontID(const JString& xFontStr, JFontID* fontID) const;
	XFont		GetXFontInfo(const JFontID id) const;

protected:

	virtual JFontID			GetFontID(const JString& name, const JSize size,
									  const JFontStyle& style) const;
	virtual const JString&	GetFontName(const JFontID id) const;
	virtual JBoolean		IsExact(const JFontID id) const;

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const;

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) const;
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str) const;

private:

	struct FontInfo
	{
		JString*	name;
		JSize		size;
		JFontStyle	style;
		XFont		xfont;
		JBoolean	exact;		// kJTrue => exact match to requested specs
		JCoordinate	ascent;		// cache for TrueType; 0 until first computed
		JCoordinate	descent;	// cache for TrueType; 0 until first computed
		JCoordinate	monoWidth;	// 0 if not monospace

		FontInfo()
			:
			name(NULL), size(0), exact(kJFalse),
			 ascent(0), descent(0), monoWidth(0)
		{ };
	};

private:

	JXDisplay*			itsDisplay;
	JArray<FontInfo>*	itsFontList;

	mutable JPtrArray<JString>*	itsAllFontNames;	// can be NULL
	mutable JPtrArray<JString>*	itsMonoFontNames;	// can be NULL

private:

	JString		BuildStdFontName(const JString& xName, const JSize size,
								 const JFontStyle& style, const JUtf8Byte* italicStr,
								 const JUtf8Byte* iso) const;
	JBoolean	BuildTrueTypeFontName(const JString& xName,
									  const JSize size, const JFontStyle& style,
									  JString* xFontStr) const;

	JBoolean	GetNewFont(const JString& name, const JSize size,
						   const JFontStyle& style, XFont* xfont) const;
	void		ApproximateFont(const JString& name, const JSize size,
								const JFontStyle& style, XFont* xfont) const;

	JString		ConvertToXFontName(const JString& name) const;
	void		ConvertToPSFontName(JString* name) const;

	JCoordinate	IsMonospace(const XFont& xfont) const;
	JBoolean	IsPostscript(const JString& name) const;
	JBoolean	IsUseless(const JString& name) const;

	// not allowed

	JXFontManager(const JXFontManager& source);
	const JXFontManager& operator=(const JXFontManager& source);
};

#endif
