/******************************************************************************
 ProgressIndicator.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ProgressIndicator
#define _H_ProgressIndicator

#include "CoreWidget.h"

class JXProgressIndicator;

class ProgressIndicator : public CoreWidget
{
public:

	ProgressIndicator(LayoutContainer* layout,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);
	ProgressIndicator(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~ProgressIndicator() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	JString	GetClassName() const override;

private:

	JXProgressIndicator*	itsWidget;

private:

	void	ProgressIndicatorX(const JCoordinate x, const JCoordinate y,
								const JCoordinate w, const JCoordinate h);
};

#endif
