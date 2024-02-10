/******************************************************************************
 TabTitleTable.h

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_TabTitleTable
#define _H_TabTitleTable

#include <jx-af/jx/JXStringTable.h>

class JIntegerTableData;

class TabTitleTable : public JXStringTable
{
public:

	TabTitleTable(JStringTableData* data,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	~TabTitleTable() override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
};

#endif
