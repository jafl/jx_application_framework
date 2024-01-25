/******************************************************************************
 Slider.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_Slider
#define _H_Slider

#include "CoreWidget.h"

class JXSliderBase;

class Slider : public CoreWidget
{
public:

	enum Type
	{
		kSliderType,
		kLevelControlType
	};

public:

	Slider(const Type type, LayoutContainer* layout,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);
	Slider(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	~Slider() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	JString	GetClassName() const override;

private:

	Type			itsType;
	JXSliderBase*	itsWidget;

private:

	void	SliderX(const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
};

#endif
