/******************************************************************************
 JXTextMenuTable.h

	Interface for the JXTextMenuTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenuTable
#define _H_JXTextMenuTable

#include <JXMenuTable.h>
#include <jXConstants.h>	// need definition of kJXDefaultBorderWidth

class JXTextMenuData;

class JXTextMenuTable : public JXMenuTable
{
public:

	enum
	{
		kHMarginWidth       = 2,
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

	virtual ~JXTextMenuTable();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual JBoolean	CellToItemIndex(const JPoint& pt, const JPoint& cell,
										JIndex* itemIndex) const;
	virtual void		MenuHilightItem(const JIndex itemIndex);
	virtual void		MenuUnhilightItem(const JIndex itemIndex);
	virtual void		GetSubmenuPoints(const JIndex itemIndex,
										 JPoint* leftPt, JPoint* rightPt);

	JRect	AdjustRectForSeparator(const JIndex rowIndex, const JRect& rect);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JXTextMenuData*	itsTextMenuData;
	JIndex					itsHilightRow;

private:

	// not allowed

	JXTextMenuTable(const JXTextMenuTable& source);
	const JXTextMenuTable& operator=(const JXTextMenuTable& source);
};

#endif
