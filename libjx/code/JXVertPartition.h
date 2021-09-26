/******************************************************************************
 JXVertPartition.h

	Interface for the JXVertPartition class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXVertPartition
#define _H_JXVertPartition

#include "jx-af/jx/JXPartition.h"

class JXVertPartition : public JXPartition
{
	friend class JXRestorePartitionGeometry;

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

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	virtual JCoordinate		GetTotalSize() const override;
	virtual JXWidgetSet*	CreateCompartment(const JIndex index,
											  const JCoordinate position,
											  const JCoordinate size) override;
	virtual void	UpdateCompartmentSizes() override;
	virtual bool	SaveGeometryForLater(const JArray<JCoordinate>& sizes) override;

	virtual bool	RunInternalFTC(const bool horizontal, JCoordinate* newSize) override;
	virtual void	FTCAdjustSize(const JCoordinate dw, const JCoordinate dh) override;

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

	// used during FTC

	JArray<JCoordinate>*	itsFTCSizes;	// nullptr unless in FTC
	JArray<JCoordinate>*	itsFTCMinSizes;	// nullptr unless in FTC
	JArray<JCoordinate>*	itsSavedGeom;	// nullptr unless in FTC w/ geometry loaded

private:

	void	JXVertPartitionX();
};

#endif
