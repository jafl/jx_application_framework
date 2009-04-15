/******************************************************************************
 JXFontNameMenu.h

	Interface for the JXFontNameMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFontNameMenu
#define _H_JXFontNameMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXFontNameMenu : public JXTextMenu
{
public:

	JXFontNameMenu(const JCharacter* title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontNameMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXFontNameMenu();

	JString		GetFontName() const;
	JBoolean	SetFontName(const JCharacter* name);

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex		itsFontIndex;
	JBoolean	itsBroadcastNameChangeFlag;

private:

	void	BuildMenu();
	JString	GetFontName(const JIndex index) const;
	void	SetFontName1(const JIndex index);

	// not allowed

	JXFontNameMenu(const JXFontNameMenu& source);
	const JXFontNameMenu& operator=(const JXFontNameMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kNameNeedsUpdate;
	static const JCharacter* kNameChanged;

	class NameNeedsUpdate : public JBroadcaster::Message
		{
		public:

			NameNeedsUpdate()
				:
				JBroadcaster::Message(kNameNeedsUpdate)
				{ };
		};

	class NameChanged : public JBroadcaster::Message
		{
		public:

			NameChanged()
				:
				JBroadcaster::Message(kNameChanged)
				{ };

		// call broadcaster's GetFont() to get fontName
		};
};


/******************************************************************************
 GetFontName

 ******************************************************************************/

inline JString
JXFontNameMenu::GetFontName()
	const
{
	return GetFontName(itsFontIndex);
}

#endif
