/******************************************************************************
 HistoryMenu.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_HistoryMenu
#define _H_HistoryMenu

#include "CoreWidget.h"

class JXTextMenu;
class HistoryMenuPanel;

class HistoryMenu : public CoreWidget
{
public:

	enum Type
	{
		kStringType,
		kFileType,
		kPathType
	};

public:

	HistoryMenu(const Type type, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	HistoryMenu(const Type type, const JString& historyLength,
				LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	HistoryMenu(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~HistoryMenu() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	JString	GetClassName() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JXTextMenu*			itsMenu;
	Type				itsType;
	JString				itsHistoryLength;
	HistoryMenuPanel*	itsPanel;		// nullptr unless editing

private:

	void	HistoryMenuX(const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);
};

#endif
