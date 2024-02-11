/******************************************************************************
 PartitionMinSizeTable.h

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_PartitionMinSizeTable
#define _H_PartitionMinSizeTable

#include <jx-af/jx/JXIntegerTable.h>

class JIntegerTableData;

class PartitionMinSizeTable : public JXIntegerTable
{
public:

	PartitionMinSizeTable(JIntegerTableData* data,
						  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						  const HSizingOption hSizing, const VSizingOption vSizing,
						  const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);

	~PartitionMinSizeTable() override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	JXIntegerInput*
		CreateIntegerTableInput(const JPoint& cell, JXContainer* enclosure,
								const HSizingOption hSizing, const VSizingOption vSizing,
								const JCoordinate x, const JCoordinate y,
								const JCoordinate w, const JCoordinate h) override;
};

#endif
