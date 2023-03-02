/******************************************************************************
 JXImageMenuTable.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageMenuTable
#define _H_JXImageMenuTable

#include "JXMenuTable.h"
#include "jXConstants.h"	// need definition of kJXDefaultBorderWidth

class JXImageMenuData;

class JXImageMenuTable : public JXMenuTable
{
public:

	enum
	{
		kHilightBorderWidth = kJXDefaultBorderWidth
	};

public:

	JXImageMenuTable(JXMenu* menu, JXImageMenuData* data, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~JXImageMenuTable() override;

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	bool	CellToItemIndex(const JPoint& pt, const JPoint& cell,
							JIndex* itemIndex) const override;
	void	MenuHilightItem(const JIndex itemIndex) override;
	void	MenuUnhilightItem(const JIndex itemIndex) override;
	void	GetSubmenuPoints(const JIndex itemIndex,
							 JPoint* leftPt, JPoint* rightPt) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JXImageMenuData*	itsIconMenuData;
	bool					itsHasCheckboxesFlag;
	bool					itsHasSubmenusFlag;
	JPoint					itsHilightCell;

private:

	JPoint	ItemIndexToCell(const JIndex itemIndex) const;
};

#endif
