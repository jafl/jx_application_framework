/******************************************************************************
 CBStylerBase.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBStylerBase
#define _H_CBStylerBase

#include <JSTStyler.h>
#include <JPrefObject.h>
#include <JStringMap.h>
#include "CBTextFileType.h"

class CBEditStylerDialog;

class CBStylerBase : public JSTStyler, public JPrefObject, virtual public JBroadcaster
{
public:

	typedef JStrValue<JFontStyle>	WordStyle;

public:

	CBStylerBase(const JSize typeVersion,
				 const JSize typeCount, const JUtf8Byte** typeNames,
				 const JString& editDialogTitle, const JPrefID& prefID,
				 const CBTextFileType fileType);

	virtual ~CBStylerBase();

	void	EditStyles();

	const JStringMap<JFontStyle>&	GetWordList() const;

	void	ReadFromSharedPrefs(std::istream& input);
	void	WriteForSharedPrefs(std::ostream& output) const;

protected:

	JFontStyle	GetStyle(const JIndex typeIndex, const JString& word);

	JFontStyle	GetTypeStyle(const JIndex index) const;
	void		SetTypeStyle(const JIndex index, const JFontStyle& style);

	bool	GetWordStyle(const JString& word, JFontStyle* style) const;
	void	SetWordStyle(const JString& word, const JFontStyle& style);
	void	RemoveWordStyle(const JString& word);

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) = 0;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JColorID				itsDefColor;

	const JFileVersion		itsTypeNameVersion;
	const JSize				itsTypeNameCount;
	const JUtf8Byte**		itsTypeNames;			// not owned
	JArray<JFontStyle>*		itsTypeStyles;
	const CBTextFileType	itsFileType;			// only use for IsCharInWord() !!

	JStringMap<JFontStyle>*	itsWordStyles;

	const JString&			itsDialogTitle;
	CBEditStylerDialog*		itsEditDialog;			// can be nullptr
	JString					itsDialogGeom;

private:

	void	GetWordList(const JStringMap<JFontStyle>& wordStyles,
						JArray<WordStyle>* wordList,
						const bool sort) const;

	JFontStyle	ReadStyle(std::istream& input);
	void		WriteStyle(std::ostream& output, const JFontStyle& style) const;

	void	ExtractStyles();
	bool	TypeStylesChanged(const JArray<JFontStyle>& newTypeStyles) const;
	bool	WordStylesChanged(const JStringMap<JFontStyle>& newWordStyles) const;

	void	SetDefaultFontColor(const JColorID color);

	static JListT::CompareResult
		CompareWords(const WordStyle& w1, const WordStyle& w2);

	// not allowed

	CBStylerBase(const CBStylerBase& source);
	const CBStylerBase& operator=(const CBStylerBase& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kWordListChanged;

	class WordListChanged : public JBroadcaster::Message
		{
		public:

			WordListChanged(const JStringMap<JFontStyle>& wordList)
				:
				JBroadcaster::Message(kWordListChanged),
				itsWordList(wordList)
				{ };

			const JStringMap<JFontStyle>&
			GetWordList() const
			{
				return itsWordList;
			};

		private:

			const JStringMap<JFontStyle>&	itsWordList;
		};
};


/******************************************************************************
 GetWordList

 ******************************************************************************/

inline const JStringMap<JFontStyle>&
CBStylerBase::GetWordList()
	const
{
	return *itsWordStyles;
}

/******************************************************************************
 Type style (protected)

 ******************************************************************************/

inline JFontStyle
CBStylerBase::GetTypeStyle
	(
	const JIndex index
	)
	const
{
	return itsTypeStyles->GetElement(index);
}

inline void
CBStylerBase::SetTypeStyle
	(
	const JIndex		index,
	const JFontStyle&	style
	)
{
	itsTypeStyles->SetElement(index, style);
}

/******************************************************************************
 Word style (protected)

 ******************************************************************************/

inline bool
CBStylerBase::GetWordStyle
	(
	const JString&	word,
	JFontStyle*		style
	)
	const
{
	return itsWordStyles->GetElement(word, style);
}

inline void
CBStylerBase::SetWordStyle
	(
	const JString&		word,
	const JFontStyle&	style
	)
{
	itsWordStyles->SetElement(word, style);
}

inline void
CBStylerBase::RemoveWordStyle
	(
	const JString& word
	)
{
	itsWordStyles->RemoveElement(word);
}

/******************************************************************************
 Shared prefs

 ******************************************************************************/

inline void
CBStylerBase::ReadFromSharedPrefs
	(
	std::istream& input
	)
{
	ReadPrefs(input);
}

inline void
CBStylerBase::WriteForSharedPrefs
	(
	std::ostream& output
	)
	const
{
	WritePrefs(output);
}

#endif
