/******************************************************************************
 JXFontManager.h

	Interface for the JXFontManager class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontManager
#define _H_JXFontManager

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JArray.h>
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
			type(kStdType), xfset(nullptr), xftt(nullptr)
		{ };

		void Free(JXDisplay* display);
	};

public:

	static void	Init();

	JXFontManager(JXDisplay* display);

	virtual ~JXFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) override;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) override;
	virtual bool	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) override;

	// for X Window System only

	void		GetXFontNames(const JRegex& regex,
							  JPtrArray<JString>* fontNames,
							  JSortXFontNamesFn compare = nullptr);
	bool	GetXFont(const JString& xFontStr, JFont** font);
	XFont		GetXFontInfo(const JFontID id);

	void		Preload(const JFontID id);

protected:

	virtual JSize	GetLineHeight(const JFontID id, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) override;

	virtual JSize	GetCharWidth(const JFontID id, const JUtf8Character& c) override;
	virtual JSize	GetStringWidth(const JFontID id, const JString& str) override;

	virtual bool	IsExact(const JFontID id) override;
	virtual bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c) override;
	virtual bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name) override;

private:

	struct FontInfo
	{
		bool	filled;
		XFont		xfont;
		bool	exact;		// true => exact match to requested specs
		JCoordinate	ascent;		// 0 until first computed
		JCoordinate	descent;	// 0 until first computed
		JCoordinate	monoWidth;	// 0 if not monospace

		FontInfo()
			:
			filled(false), exact(false), ascent(0), descent(0), monoWidth(0)
		{ };
	};

private:

	JXDisplay*			itsDisplay;
	JArray<FontInfo>*	itsFontList;

	JPtrArray<JString>*	itsAllFontNames;	// can be nullptr
	JPtrArray<JString>*	itsMonoFontNames;	// can be nullptr

private:

	FontInfo	ResolveFontID(const JFontID id);

	static JString	BuildStdFontName(const JString& xName, const JSize pointSize,
									 const JSize pixelSize, const JSize pixelWidth,
									 const JUtf8Byte* spacing,
									 const JFontStyle& style, const JUtf8Byte* italicStr);
	static bool	BuildTrueTypeFontName(const JString& xName,
										  const JSize size, const JFontStyle& style,
										  JString* xFontStr);
	static void		TrimTrueTypeFontName(JString* s);

	bool	GetNewFont(const JString& name, const JSize size,
						   const JFontStyle& style, XFont* xfont);
	void		ApproximateFont(const JString& name, const JSize size,
								const JFontStyle& style, XFont* xfont);

	static void		ConvertToPSFontName(JString* name);

	JCoordinate		IsMonospace(const XFont& xfont);
	static bool	ShouldIgnore(const JString& name);
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

/******************************************************************************
 Preload

 ******************************************************************************/

inline void
JXFontManager::Preload
	(
	const JFontID id
	)
{
	ResolveFontID(id);
}

#endif
