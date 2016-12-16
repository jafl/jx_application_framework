/******************************************************************************
 CBFnListWidget.h

	Interface for the CBFnListWidget class

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFnListWidget
#define _H_CBFnListWidget

#include <JXStringList.h>
#include "CBClass.h"		// need definition of InheritType
#include <JString.h>

class CBTreeWidget;
class JXImage;
class JXTextMenu;

class CBFnListWidget : public JXStringList
{
public:

	CBFnListWidget(const CBTreeWidget* treeWidget, const CBClass* theClass,
				   const JBoolean showInheritedFns,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);
	CBFnListWidget(std::istream& input, const JFileVersion vers,
				   JBoolean* keep, const CBTreeWidget* treeWidget,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~CBFnListWidget();

	void		Print(JPagePrinter& p);
	JBoolean	Reconnect();
	void		StreamOut(std::ostream& output) const;
	static void	SkipSetup(std::istream& input);

	const CBClass*	GetClass() const;
	const JString&	GetFunctionName(const JIndex index) const;

	JBoolean	IsShowingInheritedFns() const;
	void		ToggleShowInheritedFns();

	JBoolean	HasUnimplementedFns() const;
	void		SelectUnimplementedFns();

	void	CopySelectedFunctionNames() const;

	void	PrepareFunctionMenu(JXTextMenu* menu);
	void	SetMenuButton(const JXMouseButton button);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const CBTreeWidget*	itsTreeWidget;	// owns us
	const CBClass*		itsClass;		// not owned
	JString				itsClassName;	// for Reconnect()
	JString				itsKeyBuffer;
	JPtrArray<JString>*	itsFnNames;
	JBoolean			itsShowInheritedFnsFlag;
	JBoolean			itsHasUnimplementedFnsFlag;
	JBoolean			itsHasImagesFlag;
	JPtrArray<JXImage>*	itsImageList;	// contents not owned

	JXTextMenu*			itsFnMenu;		// NULL unless PrepareMenu() was called; not owned
	JXMouseButton		itsMenuButton;	// used when item is selected from itsFnMenu

private:

	void	CBFnListWidgetX(const CBTreeWidget* treeWidget, const JBoolean showInheritedFns);
	void	InstallClass(const CBClass* theClass);
	void	BuildFnList();
	void	BuildFnList1(const CBClass* theClass, const CBClass::InheritType maxAccess,
						 JPtrArray<JString>* names, JRunArray<JFontStyle>* styles);

	JBoolean	ClosestMatch(JIndex* fnIndex);

	void	HandleDoubleClick(const JXMouseButton button, const JIndex index) const;
	void	ViewDefinition(const JIndex rowIndex = 0) const;
	void	ViewDeclaration(const JIndex rowIndex = 0) const;

	JBoolean	GetSelectedFunction(JIndex* rowIndex) const;
	void		SelectFunction(const JIndex rowIndex);
	void		MoveSelection(const JCoordinate delta);

	// not allowed

	CBFnListWidget(const CBFnListWidget& source);
	const CBFnListWidget& operator=(const CBFnListWidget& source);
};


/******************************************************************************
 GetClass

 ******************************************************************************/

inline const CBClass*
CBFnListWidget::GetClass()
	const
{
	return itsClass;
}

/******************************************************************************
 GetFunctionName

 ******************************************************************************/

inline const JString&
CBFnListWidget::GetFunctionName
	(
	const JIndex index
	)
	const
{
	return *(itsFnNames->NthElement(index));
}

/******************************************************************************
 Show inherited functions

 ******************************************************************************/

inline JBoolean
CBFnListWidget::IsShowingInheritedFns()
	const
{
	return itsShowInheritedFnsFlag;
}

inline void
CBFnListWidget::ToggleShowInheritedFns()
{
	itsShowInheritedFnsFlag = !itsShowInheritedFnsFlag;
	BuildFnList();
}

/******************************************************************************
 HasUnimplementedFns

 ******************************************************************************/

inline JBoolean
CBFnListWidget::HasUnimplementedFns()
	const
{
	return itsHasUnimplementedFnsFlag;
}

/******************************************************************************
 SetMenuButton

 ******************************************************************************/

inline void
CBFnListWidget::SetMenuButton
	(
	const JXMouseButton button
	)
{
	itsMenuButton = button;
}

#endif
