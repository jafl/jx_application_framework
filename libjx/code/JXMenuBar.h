/******************************************************************************
 JXMenuBar.h

	Interface for the JXMenuBar class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMenuBar
#define _H_JXMenuBar

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>
#include <JPtrArray.h>

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

	virtual ~JXMenuBar();

	JSize			GetMenuCount() const;
	JXMenu*			GetMenu(const JIndex index);
	const JXMenu*	GetMenu(const JIndex index) const;

	JXTextMenu*	InsertTextMenu(const JIndex index, const JCharacter* title);
	JXTextMenu*	PrependTextMenu(const JCharacter* title);
	JXTextMenu*	AppendTextMenu(const JCharacter* title);

	JXTextMenu*	InsertTextMenu(const JIndex index, JXImage* image,
							   const JBoolean menuOwnsImage);
	JXTextMenu*	PrependTextMenu(JXImage* image, const JBoolean menuOwnsImage);
	JXTextMenu*	AppendTextMenu(JXImage* image, const JBoolean menuOwnsImage);

	void		DeleteMenu(const JIndex index);
	JBoolean	DeleteMenu(JXMenu* menu);

	void		InsertMenu(const JIndex index, JXMenu* menu);
	JBoolean	InsertMenuBefore(JXMenu* existingMenu, JXMenu* newMenu);
	JBoolean	InsertMenuAfter(JXMenu* existingMenu, JXMenu* newMenu);
	void		PrependMenu(JXMenu* menu);
	void		AppendMenu(JXMenu* menu);
	JXMenu*		RemoveMenu(const JIndex index);
	JBoolean	RemoveMenu(JXMenu* menu);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

private:

	JPtrArray<JXMenu>*	itsMenus;
	JXTextMenu*			itsOverflowMenu;			// NULL unless menus overflow bar
	JSize				itsIgnoreWidthChangedCount;	// kJTrue => in WidthChanged()

private:

	void	ClearOverflowMenu();
	void	WidthChanged();
	void	MenuWidthChanged(const JIndex index, const JCoordinate dw);

	// called by JXMenu

	void	MenuWidthChanged(JXMenu* theMenu, const JCoordinate dw);

	// not allowed

	JXMenuBar(const JXMenuBar& source);
	const JXMenuBar& operator=(const JXMenuBar& source);
};


/******************************************************************************
 PrependMenu

 ******************************************************************************/

inline JXTextMenu*
JXMenuBar::PrependTextMenu
	(
	const JCharacter* title
	)
{
	return InsertTextMenu(1, title);
}

inline JXTextMenu*
JXMenuBar::PrependTextMenu
	(
	JXImage*		image,
	const JBoolean	menuOwnsImage
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
	const JCharacter* title
	)
{
	return InsertTextMenu(GetMenuCount()+1, title);
}

inline JXTextMenu*
JXMenuBar::AppendTextMenu
	(
	JXImage*		image,
	const JBoolean	menuOwnsImage
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
