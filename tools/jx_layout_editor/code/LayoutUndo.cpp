/******************************************************************************
 LayoutUndo.cpp

	BASE CLASS = JUndo

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutUndo.h"
#include "LayoutDocument.h"
#include "LayoutContainer.h"
#include <sstream>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutUndo::LayoutUndo
	(
	LayoutDocument*	doc,
	const Type		type
	)
	:
	JUndo(),
	itsDoc(doc),
	itsType(type)
{
	std::ostringstream output;
	itsDoc->WriteTextFile(output, true);
	itsData = output.str();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LayoutUndo::~LayoutUndo()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
LayoutUndo::Undo()
{
	auto* newUndo = jnew LayoutUndo(itsDoc);

	std::istringstream input(itsData.GetBytes());
	itsDoc->ReadFile(input, true);

	LayoutDocument* doc = itsDoc;
	itsDoc->GetLayoutContainer()->GetUndoRedoChain()->ReplaceUndo(this, newUndo);	// deletes us
	doc->UpdateSaveState();
}
