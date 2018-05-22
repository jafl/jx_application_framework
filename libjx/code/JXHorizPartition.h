/******************************************************************************
 JXHorizPartition.h

	Interface for the JXHorizPartition class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXHorizPartition
#define _H_JXHorizPartition

#include <JXPartition.h>

class JXHorizPartition : public JXPartition
{
	friend class JXRestorePartitionGeometry;

public:

	JXHorizPartition(const JArray<JCoordinate>& widths, const JIndex elasticIndex,
					 const JArray<JCoordinate>& minWidths, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~JXHorizPartition();

protected:

	JXHorizPartition(JXContainer* enclosure, const JArray<JCoordinate>& widths,
					 const JIndex elasticIndex, const JArray<JCoordinate>& minWidths,
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
	virtual void			UpdateCompartmentSizes() override;
	virtual JBoolean		SaveGeometryForLater(const JArray<JCoordinate>& sizes) override;

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
	JCoordinate	itsMinDragX, itsMaxDragX;

	// used during FTC

	JArray<JCoordinate>*	itsFTCSizes;	// NULL unless in FTC
	JArray<JCoordinate>*	itsFTCMinSizes;	// NULL unless in FTC
	JArray<JCoordinate>*	itsSavedGeom;	// NULL unless in FTC w/ geometry loaded

private:

	void	JXHorizPartitionX();

	// not allowed

	JXHorizPartition(const JXHorizPartition& source);
	const JXHorizPartition& operator=(const JXHorizPartition& source);
};

#endif
