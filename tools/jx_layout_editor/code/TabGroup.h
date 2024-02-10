/******************************************************************************
 TabGroup.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TabGroup
#define _H_TabGroup

#include "MultiContainerWidget.h"

class JXTabGroup;
class TabGroupPanel;

class TabGroup : public MultiContainerWidget
{
public:

	TabGroup(LayoutContainer* layout,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);
	TabGroup(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	~TabGroup() override;

	void	StreamOut(std::ostream& output) const override;
	JString	GetEnclosureName(const LayoutWidget* widget) const override;

protected:

	bool	StealMouse(const int eventType, const JPoint& ptG,
					   const JXMouseButton button,
					   const unsigned int state) override;

	JString	GetClassName() const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JXTabGroup*		itsTabGroup;
	TabGroupPanel*	itsPanel;
};

#endif
