/******************************************************************************
 JXXFontMenu.h

	Interface for the JXXFontMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXXFontMenu
#define _H_JXXFontMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>
#include <JXFontManager.h>		// need definition of JSortXFontNamesFn

class JXXFontMenu : public JXTextMenu
{
public:

	static JBoolean Create(const JRegex& regex, JSortXFontNamesFn compare,
						   const JCharacter* title, JXContainer* enclosure,
						   const HSizingOption hSizing, const VSizingOption vSizing,
						   const JCoordinate x, const JCoordinate y,
						   const JCoordinate w, const JCoordinate h,
						   JXXFontMenu** menu);

	static JBoolean Create(const JRegex& regex, JSortXFontNamesFn compare,
						   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure,
						   JXXFontMenu** menu);

	virtual ~JXXFontMenu();

	JString	GetFontName() const;
	JFontID	GetFontID() const;
	void	SetFontName(const JCharacter* name);

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);
	void	AppendFontItem(const JCharacter* fontName, const JBoolean isRawXFontName);

protected:

	JXXFontMenu(const JPtrArray<JString>& fontNames,
				const JCharacter* title, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex		itsFontIndex;
	JBoolean	itsBroadcastFontChangeFlag;

private:

	JXXFontMenu(const JPtrArray<JString>& fontNames,
				JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	void	BuildMenu(const JPtrArray<JString>& fontNames);
	JString	GetFontName(const JIndex index) const;
	void	SetFontName1(const JIndex index);

	// not allowed

	JXXFontMenu(const JXXFontMenu& source);
	const JXXFontMenu& operator=(const JXXFontMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFontNeedsUpdate;
	static const JCharacter* kFontChanged;

	class FontNeedsUpdate : public JBroadcaster::Message
		{
		public:

			FontNeedsUpdate()
				:
				JBroadcaster::Message(kFontNeedsUpdate)
				{ };
		};

	class FontChanged : public JBroadcaster::Message
		{
		public:

			FontChanged()
				:
				JBroadcaster::Message(kFontChanged)
				{ };

		// call broadcaster's GetFont() to get fontName
		};
};

/******************************************************************************
 GetFontName

 ******************************************************************************/

inline JString
JXXFontMenu::GetFontName()
	const
{
	return GetFontName(itsFontIndex);
}

#endif
