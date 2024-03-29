/******************************************************************************
 JStyleTableData.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JStyleTableData
#define _H_JStyleTableData

#include "JAuxTableData.h"
#include "JFont.h"
#include "JString.h"

class JFontManager;

class JStyleTableData : public JAuxTableData<JFontStyle>
{
public:

	JStyleTableData(JTable* table, JFontManager* fontManager);

	~JStyleTableData() override;

	const JFont&	GetFont() const;
	void			SetFont(const JString& name, const JSize size);
	void			SetFont(const JFont& font);

	JFontStyle	GetCellStyle(const JPoint& cell) const;
	void		SetCellStyle(const JPoint& cell, const JFontStyle& style);
	void		SetAllCellStyles(const JFontStyle& style);

private:

	JFontManager*	itsFontManager;		// not owned
	JFont			itsFont;

private:

	void	AdjustToFont();

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFontChanged;

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
	return GetItem(cell);
}

#endif
