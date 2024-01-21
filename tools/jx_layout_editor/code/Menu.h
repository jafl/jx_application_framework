/******************************************************************************
 Menu.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_Menu
#define _H_Menu

#include "CoreWidget.h"

class JXTextMenu;
class MenuPanel;

class Menu : public CoreWidget
{
public:

	enum Type
	{
		kTextType,
		kImageType
	};

public:

	Menu(const Type type, LayoutContainer* layout,
		 const HSizingOption hSizing, const VSizingOption vSizing,
		 const JCoordinate x, const JCoordinate y,
		 const JCoordinate w, const JCoordinate h);
	Menu(const Type type, const JString& title, const JSize colCount,
		 LayoutContainer* layout,
		 const HSizingOption hSizing, const VSizingOption vSizing,
		 const JCoordinate x, const JCoordinate y,
		 const JCoordinate w, const JCoordinate h);
	Menu(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
		 const HSizingOption hSizing, const VSizingOption vSizing,
		 const JCoordinate x, const JCoordinate y,
		 const JCoordinate w, const JCoordinate h);

	~Menu() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	JString	GetClassName() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JXTextMenu*	itsMenu;
	Type		itsType;
	JSize		itsImageColCount;
	MenuPanel*	itsPanel;		// nullptr unless editing

private:

	void	MenuX(const JString& title,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);
};

#endif
