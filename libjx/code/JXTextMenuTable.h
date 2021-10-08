/******************************************************************************
 JXTextMenuTable.h

	Interface for the JXTextMenuTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenuTable
#define _H_JXTextMenuTable

#include "jx-af/jx/JXMenuTable.h"
#include "jx-af/jx/jXConstants.h"	// need definition of kJXDefaultBorderWidth

class JFont;
class JXTextMenuData;

class JXTextMenuTable : public JXMenuTable
{
public:

	enum
	{
		kHNMSMarginWidth    = 10,
		kSeparatorHeight    = 10,						// must be even
		kHilightBorderWidth = kJXDefaultBorderWidth,

		kCheckboxColumnIndex = 1,
		kImageColumnIndex,
		kTextColumnIndex,
		kSubmenuColumnIndex
	};

public:

	JXTextMenuTable(JXMenu* menu, JXTextMenuData* data, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~JXTextMenuTable() override;

	static void		AdjustFont(JXDisplay* display, const JIndex colIndex,
							   const JString& text, JFont* font);
	static JSize	GetTextWidth(JFontManager* fontMgr, const JFont& font,
								 const JIndex colIndex, const JString& text);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	bool	CellToItemIndex(const JPoint& pt, const JPoint& cell,
										JIndex* itemIndex) const override;
	void		MenuHilightItem(const JIndex itemIndex) override;
	void		MenuUnhilightItem(const JIndex itemIndex) override;
	void		GetSubmenuPoints(const JIndex itemIndex,
										 JPoint* leftPt, JPoint* rightPt) override;

	JRect	AdjustRectForSeparator(const JIndex rowIndex, const JRect& rect);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JXTextMenuData*	itsTextMenuData;
	JIndex					itsHilightRow;
};

#endif
