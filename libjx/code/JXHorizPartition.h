/******************************************************************************
 JXHorizPartition.h

	Interface for the JXHorizPartition class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXHorizPartition
#define _H_JXHorizPartition

#include "JXPartition.h"

class JXHorizPartition : public JXPartition
{
	friend class JXRestorePartitionGeometry;

public:

	JXHorizPartition(const JArray<JCoordinate>& widths, const JIndex elasticIndex,
					 const JArray<JCoordinate>& minWidths, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~JXHorizPartition() override;

protected:

	JXHorizPartition(JXContainer* enclosure, const JArray<JCoordinate>& widths,
					 const JIndex elasticIndex, const JArray<JCoordinate>& minWidths,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;
	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	JCoordinate		GetTotalSize() const override;
	JXWidgetSet*	CreateCompartment(const JIndex index,
											  const JCoordinate position,
											  const JCoordinate size) override;
	void			UpdateCompartmentSizes() override;
	bool		SaveGeometryForLater(const JArray<JCoordinate>& sizes) override;

	bool	RunInternalFTC(const bool horizontal, JCoordinate* newSize) override;
	void		FTCAdjustSize(const JCoordinate dw, const JCoordinate dh) override;

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

	JArray<JCoordinate>*	itsFTCSizes;	// nullptr unless in FTC
	JArray<JCoordinate>*	itsFTCMinSizes;	// nullptr unless in FTC
	JArray<JCoordinate>*	itsSavedGeom;	// nullptr unless in FTC w/ geometry loaded

private:

	void	JXHorizPartitionX();
};

#endif
