/******************************************************************************
 JXVertPartition.h

	Interface for the JXVertPartition class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXVertPartition
#define _H_JXVertPartition

#include <JXPartition.h>

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

	virtual JCoordinate		GetTotalSize() const;
	virtual JXWidgetSet*	CreateCompartment(const JIndex index,
											  const JCoordinate position,
											  const JCoordinate size);
	virtual void			UpdateCompartmentSizes();
	virtual JBoolean		SaveGeometryForLater(const JArray<JCoordinate>& sizes);

	virtual JBoolean	RunInternalFTC(const JBoolean horizontal, JCoordinate* newSize) override;
	virtual void		FTCAdjustSize(const JCoordinate dw, const JCoordinate dh) override;

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

	JArray<JCoordinate>*	itsFTCSizes;	// NULL unless in FTC
	JArray<JCoordinate>*	itsFTCMinSizes;	// NULL unless in FTC
	JArray<JCoordinate>*	itsSavedGeom;	// NULL unless in FTC w/ geometry loaded

private:

	void	JXVertPartitionX();

	// not allowed

	JXVertPartition(const JXVertPartition& source);
	const JXVertPartition& operator=(const JXVertPartition& source);
};

#endif
