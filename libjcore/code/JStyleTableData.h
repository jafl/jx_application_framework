/******************************************************************************
 JStyleTableData.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStyleTableData
#define _H_JStyleTableData

#include <JAuxTableData.h>
#include <JFont.h>
#include <JString.h>

class JFontManager;
class JColormap;

class JStyleTableData : public JAuxTableData<JFontStyle>
{
public:

	JStyleTableData(JTable* table, const JFontManager* fontManager, JColormap* colormap);

	virtual ~JStyleTableData();

	const JFont&	GetFont() const;
	void			SetFont(const JCharacter* name, const JSize size);
	void			SetFont(const JFont& font);

	JFontStyle	GetCellStyle(const JPoint& cell) const;
	void		SetCellStyle(const JPoint& cell, const JFontStyle& style);
	void		SetAllCellStyles(const JFontStyle& style);

private:

	const JFontManager*	itsFontManager;			// not owned
	JColormap*			itsColormap;			// not owned

	JFont	itsFont;

private:

	void	AdjustToFont();

	// not allowed

	JStyleTableData(const JStyleTableData& source);
	const JStyleTableData& operator=(const JStyleTableData& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFontChanged;

	class FontChanged : public JBroadcaster::Message
		{
		public:

			FontChanged()
				:
				JBroadcaster::Message(kFontChanged)
				{ };
		};
};


/******************************************************************************
 Font

 ******************************************************************************/

inline const JFont&
JStyleTableData::GetFont()
	const
{
	return itsFont;
}

/******************************************************************************
 GetCellStyle

 ******************************************************************************/

inline JFontStyle
JStyleTableData::GetCellStyle
	(
	const JPoint& cell
	)
	const
{
	return GetElement(cell);
}

#endif
