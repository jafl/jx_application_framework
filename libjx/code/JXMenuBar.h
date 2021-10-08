/******************************************************************************
 JXMenuBar.h

	Interface for the JXMenuBar class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenuBar
#define _H_JXMenuBar

#include "jx-af/jx/JXWidget.h"
#include <jx-af/jcore/JPtrArray.h>

const JSize	kJXDefaultMenuBarHeight = 30;

class JXMenu;
class JXTextMenu;
class JXImage;

class JXMenuBar : public JXWidget
{
	friend class JXMenu;

public:

	JXMenuBar(JXContainer* enclosure,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	~JXMenuBar() override;

	JSize			GetMenuCount() const;
	JXMenu*			GetMenu(const JIndex index);
	const JXMenu*	GetMenu(const JIndex index) const;
	bool			FindMenu(const JXMenu* menu, JIndex* index) const;

	JXTextMenu*	InsertTextMenu(const JIndex index, const JString& title);
	JXTextMenu*	PrependTextMenu(const JString& title);
	JXTextMenu*	AppendTextMenu(const JString& title);

	JXTextMenu*	InsertTextMenu(const JIndex index, JXImage* image,
							   const bool menuOwnsImage);
	JXTextMenu*	PrependTextMenu(JXImage* image, const bool menuOwnsImage);
	JXTextMenu*	AppendTextMenu(JXImage* image, const bool menuOwnsImage);

	void	DeleteMenu(const JIndex index);
	bool	DeleteMenu(JXMenu* menu);

	void	InsertMenu(const JIndex index, JXMenu* menu);
	bool	InsertMenuBefore(JXMenu* existingMenu, JXMenu* newMenu);
	bool	InsertMenuAfter(JXMenu* existingMenu, JXMenu* newMenu);
	void	PrependMenu(JXMenu* menu);
	void	AppendMenu(JXMenu* menu);
	JXMenu*	RemoveMenu(const JIndex index);
	bool	RemoveMenu(JXMenu* menu);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

private:

	JPtrArray<JXMenu>*	itsMenus;
	JXTextMenu*			itsOverflowMenu;			// nullptr unless menus overflow bar
	JSize				itsIgnoreWidthChangedCount;	// true => in WidthChanged()

private:

	void	ClearOverflowMenu();
	void	WidthChanged();
	void	MenuWidthChanged(const JIndex index, const JCoordinate dw);

	// called by JXMenu

	void	MenuWidthChanged(JXMenu* theMenu, const JCoordinate dw);
};


/******************************************************************************
 PrependMenu

 ******************************************************************************/

inline JXTextMenu*
JXMenuBar::PrependTextMenu
	(
	const JString& title
	)
{
	return InsertTextMenu(1, title);
}

inline JXTextMenu*
JXMenuBar::PrependTextMenu
	(
	JXImage*		image,
	const bool	menuOwnsImage
	)
{
	return InsertTextMenu(1, image, menuOwnsImage);
}

inline void
JXMenuBar::PrependMenu
	(
	JXMenu* menu
	)
{
	InsertMenu(1, menu);
}

/******************************************************************************
 AppendMenu

 ******************************************************************************/

inline JXTextMenu*
JXMenuBar::AppendTextMenu
	(
	const JString& title
	)
{
	return InsertTextMenu(GetMenuCount()+1, title);
}

inline JXTextMenu*
JXMenuBar::AppendTextMenu
	(
	JXImage*		image,
	const bool	menuOwnsImage
	)
{
	return InsertTextMenu(GetMenuCount()+1, image, menuOwnsImage);
}

inline void
JXMenuBar::AppendMenu
	(
	JXMenu* menu
	)
{
	InsertMenu(GetMenuCount()+1, menu);
}

#endif
