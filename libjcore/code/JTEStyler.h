/******************************************************************************
 JTEStyler.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEStyler
#define _H_JTEStyler

#include "JTextEditor.h"	// need def of Font
#include <FlexLexer.h>

class JTEStyler
{
public:

	// Feel free to request other items in this union.  It is only here
	// because certain exceptional languages like PHP require additional
	// state.  We don't provide a void* because then we would have to keep
	// track of whether or not it needs to be deleted.

	union TokenExtra
	{
		yy_state_type lexerState;
	};

	struct TokenData
	{
		JTextEditor::TextIndex	startIndex;
		TokenExtra				data;

		TokenData()
			:
			data()
		{ };

		TokenData(const JTextEditor::TextIndex& i, const TokenExtra& d)
			:
			startIndex(i), data(d)
		{ };
	};

public:

	JTEStyler();

	virtual ~JTEStyler();

	static JArray<TokenData>*	NewTokenStartList();

	void	UpdateStyles(const JTextEditor* te,
						 const JString& text, JRunArray<JFont>* styles,
						 JIndexRange* recalcRange, JIndexRange* redrawRange,
						 const JBoolean deletion, JArray<TokenData>* tokenStartList);

	JBoolean	IsActive() const;
	void		SetActive(const JBoolean active);

protected:

	virtual void		Scan(std::istream& input, const TokenExtra& initData) = 0;
	virtual TokenExtra	GetFirstTokenExtraData() const;
	virtual void		PreexpandCheckRange(const JString& text,
											const JRunArray<JFont>& styles,
											const JIndexRange& modifiedRange,
											const JBoolean deletion,
											JIndexRange* checkRange);
	void				ExtendCheckRange(const JIndex newEndIndex);

	const JTextEditor*		GetTextEditor() const;
	JFontManager*			GetFontManager() const;
	const JFont&			GetDefaultFont() const;
	const JString&			GetText() const;
	const JRunArray<JFont>&	GetStyles() const;

	JBoolean	SetStyle(const JIndexRange& range, const JFontStyle& style);
	void		SaveTokenStart(const TokenExtra& data);

	void	AdjustStyle(const JIndexRange& range, const JFontStyle& style);

private:

	JBoolean	itsActiveFlag;

	const JTextEditor*	itsTE;			// not owned; nullptr unless lexing
	JFontManager*		itsFontMgr;		// not owned; nullptr unless lexing
	const JString*		itsText;		// not owned; nullptr unless lexing
	JRunArray<JFont>*	itsStyles;		// not owned; nullptr unless lexing

	JBoolean	itsRedoAllFlag;						// kJTrue => itsStyles is *not* full
	JFont*		itsDefFont;							// nullptr unless processing

	JIndexRange*	itsRecalcRange;					// not owned; nullptr unless lexing
	JIndexRange*	itsRedrawRange;					// not owned; nullptr unless lexing
	JIndexRange		itsCheckRange;

	JArray<TokenData>*	itsTokenStartList;			// not owned; nullptr unless lexing
	JSize				itsTokenStartFactor;
	JSize				itsTokenStartCount;

	// information about next token that will be received

	JIndex	itsTokenStart;
	JIndex	itsTokenRunIndex;						// invalid when itsRedoAllFlag
	JIndex	itsTokenFirstInRun;						// invalid when itsRedoAllFlag

private:

	JBoolean	OnlyColorChanged(JFontStyle s1, JFontStyle s2) const;

	static JListT::CompareResult
		CompareTokenStarts(const TokenData& t1, const TokenData& t2);

	// not allowed

	JTEStyler(const JTEStyler& source);
	const JTEStyler& operator=(const JTEStyler& source);
};


/******************************************************************************
 Active

 ******************************************************************************/

inline JBoolean
JTEStyler::IsActive()
	const
{
	return itsActiveFlag;
}

inline void
JTEStyler::SetActive
	(
	const JBoolean active
	)
{
	itsActiveFlag = active;
}

/******************************************************************************
 GetTextEditor (protected)

 ******************************************************************************/

inline const JTextEditor*
JTEStyler::GetTextEditor()
	const
{
	return itsTE;
}

/******************************************************************************
 GetFontManager (protected)

 ******************************************************************************/

inline JFontManager*
JTEStyler::GetFontManager()
	const
{
	return itsFontMgr;
}

/******************************************************************************
 GetDefaultFont (protected)

 ******************************************************************************/

inline const JFont&
JTEStyler::GetDefaultFont()
	const
{
	return *itsDefFont;
}

/******************************************************************************
 GetText (protected)

 ******************************************************************************/

inline const JString&
JTEStyler::GetText()
	const
{
	return *itsText;
}

/******************************************************************************
 GetStyles (protected)

 ******************************************************************************/

inline const JRunArray<JFont>&
JTEStyler::GetStyles()
	const
{
	return *itsStyles;
}

#endif
