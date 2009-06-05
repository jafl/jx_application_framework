/******************************************************************************
 JXToolBarButton.h

	Copyright � 1998 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JXToolBarButton
#define _H_JXToolBarButton

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXButton.h>
#include <JString.h>
#include <JFontStyle.h>

class JXToolBar;
class JXTextMenu;
class JXImage;

class JXToolBarButton : public JXButton
{
public:

	enum Type
	{
		kImage = 1,
		kText,
		kBoth
	};

public:

	JXToolBarButton(JXToolBar* toolbar, JXTextMenu* menu, const JCharacter* itemID,
					const Type type, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate h);

	virtual ~JXToolBarButton();

	JXTextMenu*		GetMenu() const;
	const JString&	GetMenuItemID() const;
	JBoolean		GetMenuItemIndex(JIndex* itemIndex) const;

	virtual void	Activate();
	virtual void	Deactivate();

	virtual void	Suspend();
	virtual void	Resume();

	JBoolean		NeedsGeometryAdjustment(JBoolean* invalid);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

private:

	JXToolBar*	itsToolBar;			// owns us
	JXTextMenu*	itsMenu;			// not owned
	JString		itsMenuItemID;
	JBoolean	itsIsCheckedFlag;	// last known state of menu item

	Type		itsType;

	JString		itsLabel;
	JColorIndex	itsTrueLabelColor;
	JString		itsFontName;
	JSize		itsFontSize;
	JFontStyle	itsFontStyle;

	const JXImage*	itsImage;

private:

	void	CloseHints();

	// not allowed

	JXToolBarButton(const JXToolBarButton& source);
	const JXToolBarButton& operator=(const JXToolBarButton& source);
};


/******************************************************************************
 GetMenu

 ******************************************************************************/

inline JXTextMenu*
JXToolBarButton::GetMenu()
	const
{
	return itsMenu;
}

/******************************************************************************
 GetMenuItemID

 ******************************************************************************/

inline const JString&
JXToolBarButton::GetMenuItemID()
	const
{
	return itsMenuItemID;
}

#endif
