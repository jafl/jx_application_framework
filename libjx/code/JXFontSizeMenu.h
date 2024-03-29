/******************************************************************************
 JXFontSizeMenu.h

	Interface for the JXFontSizeMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontSizeMenu
#define _H_JXFontSizeMenu

#include "JXTextMenu.h"

class JXFontNameMenu;

class JXFontSizeMenu : public JXTextMenu
{
public:

	JXFontSizeMenu(const JString& fontName, const JString& title,
				   JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontSizeMenu(const JString& fontName, JXMenu* owner,
				   const JIndex itemIndex, JXContainer* enclosure);

	JXFontSizeMenu(JXFontNameMenu* fontMenu, const JString& title,
				   JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontSizeMenu(JXFontNameMenu* fontMenu, JXMenu* owner,
				   const JIndex itemIndex, JXContainer* enclosure);

	~JXFontSizeMenu() override;

	void	SetFontName(const JString& name);

	JSize	GetFontSize() const;
	void	SetFontSize(const JSize size);

	using JXTextMenu::SetToPopupChoice;

	void	SetToPopupChoice(const bool isPopup = true);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFontNameMenu*	itsFontNameMenu;	// can be nullptr
	JSize			itsFontSize;
	JIndex			itsCurrIndex;
	JIndex			itsVarSizeIndex;	// 0 if font not rescalable
	bool			itsBroadcastChangeFlag;

private:

	void	JXFontSizeMenuX(JXFontNameMenu* fontMenu);
	void	BuildMenu(const JString& fontName);
	void	AdjustVarSizeItem(const JSize fontSize);
	void	ChooseFontSize(const JIndex sizeIndex);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kSizeNeedsUpdate;
	static const JUtf8Byte* kSizeChanged;

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
	const JString& name
	)
{
	const JSize currSize = GetFontSize();
	BuildMenu(name);
	SetFontSize(currSize);
}

#endif
