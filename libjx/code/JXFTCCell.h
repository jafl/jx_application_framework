/******************************************************************************
 JXFTCCell.h

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFTCCell
#define _H_JXFTCCell

#include <JXContainer.h>

class JXFTCCell : public JXContainer
{
public:

	enum Direction
	{
		kNoDirection,
		kHorizontal,
		kVertical
	};

public:

	JXFTCCell(JXContainer* matchObj, JXContainer* enc, const Direction direction);

	virtual ~JXFTCCell();

	JXContainer*	GetWidget();
	JSize			GetDepth() const;
	Direction		GetDirection() const;

	JCoordinate		Expand(const JBoolean horizontal);

	virtual void	Refresh() const;
	virtual void	Redraw() const;

	virtual JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const;
	virtual JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const;

	virtual void	Place(const JCoordinate enclX, const JCoordinate enclY);
	virtual void	Move(const JCoordinate dx, const JCoordinate dy);
	virtual void	SetSize(const JCoordinate w, const JCoordinate h);
	virtual void	AdjustSize(const JCoordinate dw, const JCoordinate dh);

	virtual JRect	GetBoundsGlobal() const;
	virtual JRect	GetFrameGlobal() const;
	virtual JRect	GetApertureGlobal() const;

	virtual JString	ToString() const;

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void	DrawBackground(JXWindowPainter& p, const JRect& frame);

	// position and size adjustments -- must call inherited

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh);

private:

	JXContainer*			itsWidget;			// can be NULL
	JRect					itsFrameG;			// global coords
	Direction				itsDirection;		// direction of contained cell stack
	JBoolean				itsSyncChildrenFlag;
	JPtrArray<JXFTCCell>*	itsChildren;
	JArray<JCoordinate>*	itsChildSpacing;
	JArray<JFloat>*			itsChildPositions;	// -1: left, +1:right, 0:both, else:center-line:0<v<1

private:

	void		BuildChildList(const JBoolean horizontal);
	void		ComputeInvariants(const JBoolean horizontal);
	void		EnforceInvariants(const JBoolean horizontal);
	void		CoverChildren();
	JCoordinate	ExpandWidget(const JBoolean horizontal);
	void		SyncWidgetPosition();
	void		SyncWidgetSize();

	JString	Indent(const JSize extra = 0) const;

	static JOrderedSetT::CompareResult
		CompareHorizontally(JXFTCCell* const & c1, JXFTCCell* const & c2);
	static JOrderedSetT::CompareResult
		CompareVertically(JXFTCCell* const & c1, JXFTCCell* const & c2);

	// not allowed

	JXFTCCell(const JXFTCCell& source);
	const JXFTCCell& operator=(const JXFTCCell& source);
};


/******************************************************************************
 GetWidget

 ******************************************************************************/

inline JXContainer*
JXFTCCell::GetWidget()
{
	return itsWidget;
}

/******************************************************************************
 GetDirection

 ******************************************************************************/

inline JXFTCCell::Direction
JXFTCCell::GetDirection()
	const
{
	return itsDirection;
}

#endif
