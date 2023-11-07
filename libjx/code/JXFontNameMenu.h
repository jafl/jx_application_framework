/******************************************************************************
 JXFontNameMenu.h

	Interface for the JXFontNameMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontNameMenu
#define _H_JXFontNameMenu

#include "JXTextMenu.h"

class JXFontNameMenu : public JXTextMenu
{
public:

	JXFontNameMenu(const JString& title, const bool prependHistory,
				   JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontNameMenu(const bool prependHistory,
				   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXFontNameMenu() override;

	JString	GetFontName() const;
	bool	SetFontName(const JString& name);

	using JXTextMenu::SetToPopupChoice;

	void	SetToPopupChoice(const bool isPopup = true);

	JSize	GetHistoryCount() const;

protected:

	JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsFontIndex;
	bool	itsBroadcastNameChangeFlag;

	JPtrArray<JString>*	itsNameHistory;		// nullptr if not prepending history

private:

	void	BuildMenu(const bool prependHistory);
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
	return itsNameHistory->GetItemCount();
}

#endif
