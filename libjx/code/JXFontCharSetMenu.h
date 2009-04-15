/******************************************************************************
 JXFontCharSetMenu.h

	Interface for the JXFontCharSetMenu class

	Copyright © 1996-2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFontCharSetMenu
#define _H_JXFontCharSetMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXFontNameMenu;
class JXFontSizeMenu;

class JXFontCharSetMenu : public JXTextMenu
{
public:

	JXFontCharSetMenu(const JCharacter* fontName, const JSize fontSize,
					  const JCharacter* title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXFontCharSetMenu(const JCharacter* fontName, const JSize fontSize,
					  JXMenu* owner, const JIndex itemIndex,
					  JXContainer* enclosure);

	JXFontCharSetMenu(JXFontNameMenu* fontMenu, JXFontSizeMenu* sizeMenu,
					  const JCharacter* title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXFontCharSetMenu(JXFontNameMenu* fontMenu, JXFontSizeMenu* sizeMenu,
					  JXMenu* owner, const JIndex itemIndex,
					  JXContainer* enclosure);

	virtual ~JXFontCharSetMenu();

	void	SetFont(const JCharacter* name, const JSize size);

	const JString&	GetCharSet() const;
	JBoolean		SetCharSet(const JCharacter* charSet);
	void			SelectDefaultCharSet();

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXFontNameMenu*	itsFontNameMenu;	// can be NULL
	JXFontSizeMenu*	itsFontSizeMenu;	// can be NULL
	JString			itsCharSet;
	JIndex			itsCurrIndex;
	JBoolean		itsBroadcastChangeFlag;

private:

	void	JXFontCharSetMenuX(JXFontNameMenu* fontMenu, JXFontSizeMenu* sizeMenu);
	void	BuildMenu(const JCharacter* fontName, const JSize fontSize);
	void	ChooseCharSet(const JIndex index);

	// not allowed

	JXFontCharSetMenu(const JXFontCharSetMenu& source);
	const JXFontCharSetMenu& operator=(const JXFontCharSetMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kCharSetNeedsUpdate;
	static const JCharacter* kCharSetChanged;

	class CharSetNeedsUpdate : public JBroadcaster::Message
		{
		public:

			CharSetNeedsUpdate()
				:
				JBroadcaster::Message(kCharSetNeedsUpdate)
				{ };
		};

	class CharSetChanged : public JBroadcaster::Message
		{
		public:

			CharSetChanged(const JString& charSet)
				:
				JBroadcaster::Message(kCharSetChanged),
				itsCharSet(charSet)
				{ };

			const JString&
			GetSize() const
			{
				return itsCharSet;
			};

		private:

			const JString&	itsCharSet;
		};
};

/******************************************************************************
 GetCharSet

 ******************************************************************************/

inline const JString&
JXFontCharSetMenu::GetCharSet()
	const
{
	return itsCharSet;
}

#endif
