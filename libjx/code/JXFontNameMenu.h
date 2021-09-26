/******************************************************************************
 JXFontNameMenu.h

	Interface for the JXFontNameMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontNameMenu
#define _H_JXFontNameMenu

#include "jx-af/jx/JXTextMenu.h"

class JXFontNameMenu : public JXTextMenu
{
public:

	JXFontNameMenu(const JString& title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontNameMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXFontNameMenu();

	JString		GetFontName() const;
	bool	SetFontName(const JString& name);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

	void	SetToPopupChoice(const bool isPopup = true);

#pragma GCC diagnostic pop

	JSize	GetHistoryCount() const;

protected:

	virtual JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex		itsFontIndex;
	bool	itsBroadcastNameChangeFlag;

	JPtrArray<JString>*	itsNameHistory;

private:

	void	BuildMenu();
	void	UpdateMenu();
	void	UpdateHistory(const JString& name);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNameNeedsUpdate;
	static const JUtf8Byte* kNameChanged;

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
	return GetItemText(itsFontIndex);
}

/******************************************************************************
 GetHistoryCount

 ******************************************************************************/

inline JSize
JXFontNameMenu::GetHistoryCount()
	const
{
	return itsNameHistory->GetElementCount();
}

#endif
