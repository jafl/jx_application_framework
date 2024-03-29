/******************************************************************************
 JXToolBarButton.h

	Copyright (C) 1998 by Glenn W. Bach.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JXToolBarButton
#define _H_JXToolBarButton

#include "JXButton.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JFont.h>

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

	JXToolBarButton(JXToolBar* toolbar, JXTextMenu* menu, const JString& itemID,
					const Type type, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate h);

	~JXToolBarButton() override;

	JXTextMenu*		GetMenu() const;
	const JString&	GetMenuItemID() const;
	bool			GetMenuItemIndex(JIndex* itemIndex) const;

	bool			NeedsGeometryAdjustment(bool* invalid);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
								   const JPoint& pt, const Time time,
								   const JXWidget* source) override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
								  const Atom action, const Time time,
								  const JXWidget* source) override;

private:

	JXToolBar*	itsToolBar;			// owns us
	JXTextMenu*	itsMenu;			// not owned
	JString		itsMenuItemID;
	bool		itsIsCheckedFlag;	// last known state of menu item

	Type		itsType;

	JString		itsLabel;
	JFont		itsFont;

	const JXImage*	itsImage;

private:

	JFont	GetLabelFont() const;
	void	CloseHints();
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
