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

	enum Type
	{
		kUnclassifiedType,
		kWindowResizeType,
		kArrowType,
		kDragResizeType,
		kResizePartitionType
	};

public:

	LayoutUndo(LayoutDocument* doc, const Type type = kUnclassifiedType);

	~LayoutUndo() override;

	void	Undo() override;
	Type	GetType() const;

private:

	LayoutDocument*	itsDoc;		// not owned
	JString			itsData;
	const Type		itsType;
};


/******************************************************************************
 IsArrow

 ******************************************************************************/

inline LayoutUndo::Type
LayoutUndo::GetType()
	const
{
	return itsType;
}

#endif
