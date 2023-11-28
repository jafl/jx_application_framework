/******************************************************************************
 MenuItemSelection.h

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_MenuItemSelection
#define _H_MenuItemSelection

#include <jx-af/jx/JXSelectionManager.h>
#include "MenuTable.h"

class MenuTable;

class MenuItemSelection : public JXSelectionData
{
public:

	MenuItemSelection(JXDisplay* display, MenuTable* table,
					  const MenuTable::ItemInfo& itemInfo);

	~MenuItemSelection() override;

	static const JUtf8Byte*	GetMenuItemXAtomName();

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;

private:

	JString		itsData;
	MenuTable*	itsTable;	// not owned; can be nullptr
	JIndex		itsSrcRowIndex;

	Atom	itsMenuItemXAtom;
};

#endif
