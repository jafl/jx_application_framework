/******************************************************************************
 JXFontSizeMenu.h

	Interface for the JXFontSizeMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFontSizeMenu
#define _H_JXFontSizeMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXFontNameMenu;
class JXChooseFontSizeDialog;

class JXFontSizeMenu : public JXTextMenu
{
public:

	JXFontSizeMenu(const JCharacter* fontName, const JCharacter* title,
				   JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontSizeMenu(const JCharacter* fontName, JXMenu* owner,
				   const JIndex itemIndex, JXContainer* enclosure);

	JXFontSizeMenu(JXFontNameMenu* fontMenu, const JCharacter* title,
				   JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontSizeMenu(JXFontNameMenu* fontMenu, JXMenu* owner,
				   const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXFontSizeMenu();

	void	SetFontName(const JCharacter* name);

	JSize	GetFontSize() const;
	void	SetFontSize(const JSize size);

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXFontNameMenu*	itsFontNameMenu;	// can be NULL
	JSize			itsFontSize;
	JIndex			itsCurrIndex;
	JIndex			itsVarSizeIndex;	// 0 if font not rescalable
	JBoolean		itsBroadcastChangeFlag;

	// used when selecting arbitrary font size

	JXChooseFontSizeDialog*	itsChooseSizeDialog;	// can be NULL

private:

	void	JXFontSizeMenuX(JXFontNameMenu* fontMenu);
	void	BuildMenu(const JCharacter* fontName);
	void	AdjustVarSizeItem(const JSize fontSize);
	void	ChooseFontSize(const JIndex sizeIndex);

	// not allowed

	JXFontSizeMenu(const JXFontSizeMenu& source);
	const JXFontSizeMenu& operator=(const JXFontSizeMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kSizeNeedsUpdate;
	static const JCharacter* kSizeChanged;

	class SizeNeedsUpdate : public JBroadcaster::Message
		{
		public:

			SizeNeedsUpdate()
				:
				JBroadcaster::Message(kSizeNeedsUpdate)
				{ };
		};

	class SizeChanged : public JBroadcaster::Message
		{
		public:

			SizeChanged(const JSize size)
				:
				JBroadcaster::Message(kSizeChanged),
				itsSize(size)
				{ };

			JSize
			GetSize() const
			{
				return itsSize;
			};

		private:

			JSize itsSize;
		};
};

/******************************************************************************
 GetFontSize

 ******************************************************************************/

inline JSize
JXFontSizeMenu::GetFontSize()
	const
{
	return itsFontSize;
}

/******************************************************************************
 SetFontName

 ******************************************************************************/

inline void
JXFontSizeMenu::SetFontName
	(
	const JCharacter* name
	)
{
	const JSize currSize = GetFontSize();
	BuildMenu(name);
	SetFontSize(currSize);
}

#endif
