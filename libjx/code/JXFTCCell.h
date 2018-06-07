/******************************************************************************
 JXFTCCell.h

	Copyright (C) 2017 by John Lindal.

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

	JXFTCCell(JXContainer* matchObj, JXContainer* enc, const Direction direction,
			  const JBoolean exact);

	virtual ~JXFTCCell();

	JXContainer*	GetWidget();
	JSize			GetDepth() const;
	Direction		GetDirection() const;
	JBoolean		IsExact() const;

	JCoordinate		Expand(const JBoolean horizontal);

	virtual void	Refresh() const override;
	virtual void	Redraw() const override;

	virtual JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const override;
	virtual JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const override;

	virtual void	Place(const JCoordinate enclX, const JCoordinate enclY) override;
	virtual void	Move(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	SetSize(const JCoordinate w, const JCoordinate h) override;
	virtual void	AdjustSize(const JCoordinate dw, const JCoordinate dh) override;

	virtual JRect	GetBoundsGlobal() const override;
	virtual JRect	GetFrameGlobal() const override;
	virtual JRect	GetApertureGlobal() const override;

	virtual JString	ToString() const override;

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	virtual void	DrawBackground(JXWindowPainter& p, const JRect& frame) override;

	// position and size adjustments -- must call inherited

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh) override;

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
	const JBoolean			itsIsExactFlag;
	Elastic					itsElasticFlag;
	JBoolean				itsSyncChildrenFlag;
	JBoolean				itsSyncHorizontalFlag;
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
	JBoolean	IsElastic();

	JString				Indent(const JSize extra = 0) const;
	const JUtf8Byte*	GetDirectionName() const;

	static JListT::CompareResult
		CompareHorizontally(JXFTCCell* const & c1, JXFTCCell* const & c2);
	static JListT::CompareResult
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

/******************************************************************************
 IsExact

 ******************************************************************************/

inline JBoolean
JXFTCCell::IsExact()
	const
{
	return itsIsExactFlag;
}

#endif
