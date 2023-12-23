/******************************************************************************
 LayoutUndo.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutUndo
#define _H_LayoutUndo

#include <jx-af/jcore/JUndo.h>
#include <jx-af/jcore/JString.h>

class LayoutDocument;

class LayoutUndo : public JUndo
{
public:

	LayoutUndo(LayoutDocument* doc);

	~LayoutUndo() override;

	void	Undo() override;

private:

	LayoutDocument*	itsDoc;		// not owned
	JString			itsData;
};

#endif
