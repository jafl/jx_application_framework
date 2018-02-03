/******************************************************************************
 SyGHeaderWidget.h

	Interface for the SyGHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_SyGHeaderWidget
#define _H_SyGHeaderWidget

#include "JXColHeaderWidget.h"

class SyGFileTreeTable;

class SyGHeaderWidget : public JXColHeaderWidget
{
public:

	SyGHeaderWidget(SyGFileTreeTable* table,
					JXScrollbarSet* scrollbarSet,
					JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~SyGHeaderWidget();

	static JCoordinate	GetPreferredHeight(const JFontManager* fontMgr);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	SyGFileTreeTable*	itsTable;	// we don't own this

private:

	// not allowed

	SyGHeaderWidget(const SyGHeaderWidget& source);
	const SyGHeaderWidget& operator=(const SyGHeaderWidget& source);
};

#endif
