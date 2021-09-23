/******************************************************************************
 JXProgressIndicator.h

	Interface for the JXProgressIndicator class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXProgressIndicator
#define _H_JXProgressIndicator

#include "JXWidget.h"

class JXProgressIndicator : public JXWidget
{
public:

	JXProgressIndicator(JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JXProgressIndicator();

	JSize	GetValue() const;
	void	SetValue(const JSize value);
	void	IncrementValue(const JSize delta = 1);

	JSize	GetMaxValue() const;
	void	SetMaxValue(const JSize maxValue);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;	
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	JSize		itsValue;
	JSize		itsMaxValue;
	JColorID	itsForeColor;
	JColorID	itsBackColor;
};


/******************************************************************************
 GetValue

 ******************************************************************************/

inline JSize
JXProgressIndicator::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 IncrementValue

 ******************************************************************************/

inline void
JXProgressIndicator::IncrementValue
	(
	const JSize delta
	)
{
	SetValue(itsValue + delta);
}

/******************************************************************************
 GetMaxValue

 ******************************************************************************/

inline JSize
JXProgressIndicator::GetMaxValue()
	const
{
	return itsMaxValue;
}

#endif
