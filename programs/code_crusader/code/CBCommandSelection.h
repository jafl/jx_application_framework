/******************************************************************************
 CBCommandSelection.h

	Copyright (C) 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCommandSelection
#define _H_CBCommandSelection

#include <JXSelectionManager.h>
#include "CBCommandManager.h"

class CBCommandTable;

class CBCommandSelection : public JXSelectionData
{
public:

	CBCommandSelection(JXDisplay* display, CBCommandTable* table,
					   const CBCommandManager::CmdInfo& cmdInfo);

	virtual	~CBCommandSelection();

	static const JCharacter*	GetCommandXAtomName();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	JString			itsData;
	CBCommandTable*	itsTable;	// not owned; can be NULL
	JIndex			itsSrcRowIndex;

	Atom	itsCBCommandXAtom;

private:

	// not allowed

	CBCommandSelection(const CBCommandSelection& source);
	const CBCommandSelection& operator=(const CBCommandSelection& source);
};

#endif
