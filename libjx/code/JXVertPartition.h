/******************************************************************************
 JXVertPartition.h

	Interface for the JXVertPartition class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXVertPartition
#define _H_JXVertPartition

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPartition.h>

class JXVertPartition : public JXPartition
{
public:

	JXVertPartition(const JArray<JCoordinate>& heights, const JIndex elasticIndex,
					const JArray<JCoordinate>& minHeights, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~JXVertPartition();

protected:

	JXVertPartition(JXContainer* enclosure, const JArray<JCoordinate>& heights,
					const JIndex elasticIndex, const JArray<JCoordinate>& minHeights,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);

	virtual JCoordinate		GetTotalSize() const;
	virtual JXWidgetSet*	CreateCompartment(const JIndex index,
											  const JCoordinate position,
											  const JCoordinate size);
	virtual void			SetCompartmentSizes();

private:

	enum DragType
	{
		kInvalidDrag,
		kDragOne,
		kDragAll
	};

private:

	JCursorIndex	itsDragAllLineCursor;

	// used during drag

	DragType	itsDragType;
	JPoint		itsPrevPt;
	JCoordinate	itsMinDragY, itsMaxDragY;

private:

	void	JXVertPartitionX();

	// not allowed

	JXVertPartition(const JXVertPartition& source);
	const JXVertPartition& operator=(const JXVertPartition& source);
};

#endif
