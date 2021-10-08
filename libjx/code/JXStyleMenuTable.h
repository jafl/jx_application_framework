/******************************************************************************
 JXStyleMenuTable.h

	Interface for the JXStyleMenuTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStyleMenuTable
#define _H_JXStyleMenuTable

#include "jx-af/jx/JXTextMenuTable.h"

class JXStyleMenu;

class JXStyleMenuTable : public JXTextMenuTable
{
public:

	JXStyleMenuTable(JXStyleMenu* menu, JXTextMenuData* data, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~JXStyleMenuTable() override;

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

private:

	JXStyleMenu*	itsMenu;	// it owns us
};

#endif
