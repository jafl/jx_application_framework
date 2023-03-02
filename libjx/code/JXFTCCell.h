/******************************************************************************
 JXFTCCell.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFTCCell
#define _H_JXFTCCell

#include "JXContainer.h"

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

	JXFTCCell(JXContainer* matchObj, JXContainer* enc, const Direction direction,
			  const bool exact);

	~JXFTCCell() override;

	JXContainer*	GetWidget();
	JSize			GetDepth() const;
	Direction		GetDirection() const;
	bool			IsExact() const;

	JCoordinate		Expand(const bool horizontal);

	void	Refresh() const override;
	void	Redraw() const override;

	JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const override;
	JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const override;

	void	Place(const JCoordinate enclX, const JCoordinate enclY) override;
	void	Move(const JCoordinate dx, const JCoordinate dy) override;
	void	SetSize(const JCoordinate w, const JCoordinate h) override;
	void	AdjustSize(const JCoordinate dw, const JCoordinate dh) override;

	JRect	GetBoundsGlobal() const override;
	JRect	GetFrameGlobal() const override;
	JRect	GetApertureGlobal() const override;

	JString	ToString() const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	DrawBackground(JXWindowPainter& p, const JRect& frame) override;

	// position and size adjustments -- must call inherited

	void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	enum Elastic
	{
		kUnknown,
		kTrue,
		kFalse
	};

private:

	JXContainer*			itsWidget;			// can be nullptr
	JRect					itsFrameG;			// global coords
	const Direction			itsDirection;		// direction of contained cell stack
	const bool				itsIsExactFlag;
	Elastic					itsElasticFlag;
	bool					itsSyncChildrenFlag;
	bool					itsSyncHorizontalFlag;
	JPtrArray<JXFTCCell>*	itsChildren;
	JArray<JCoordinate>*	itsChildSpacing;
	JArray<JFloat>*			itsChildPositions;	// -1: left, +1:right, 0:both, else:center-line:0<v<1

private:

	void		BuildChildList();
	void		ComputeInvariants();
	void		EnforceSpacing();
	void		EnforcePositions();
	void		CoverChildren();
	JCoordinate	ExpandWidget();
	void		SyncWidgetPosition();
	void		SyncSize(const JCoordinate dw, const JCoordinate dh);
	bool		IsElastic();

	JString				Indent(const JSize extra = 0) const;
	const JUtf8Byte*	GetDirectionName() const;

	static JListT::CompareResult
		CompareHorizontally(JXFTCCell* const & c1, JXFTCCell* const & c2);
	static JListT::CompareResult
		CompareVertically(JXFTCCell* const & c1, JXFTCCell* const & c2);
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

/******************************************************************************
 IsExact

 ******************************************************************************/

inline bool
JXFTCCell::IsExact()
	const
{
	return itsIsExactFlag;
}

#endif
