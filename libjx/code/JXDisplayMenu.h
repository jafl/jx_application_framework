/******************************************************************************
 JXDisplayMenu.h

	Interface for the JXDisplayMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDisplayMenu
#define _H_JXDisplayMenu

#include "JXTextMenu.h"

class JXDisplayMenu : public JXTextMenu
{
public:

	JXDisplayMenu(const JString& title, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	JXDisplayMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXDisplayMenu() override;

	void		SelectCurrentDisplay() const;
	JXDisplay*	GetSelectedDisplay() const;
	JIndex		GetSelectedDisplayIndex() const;

	using JXTextMenu::SetToPopupChoice;

	void	SetToPopupChoice(const bool isPopup = true);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsDisplayIndex;
	JIndex	itsNewDisplayIndex;

private:

	void	BuildMenu();
	void	ChooseDisplay(const JIndex index);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kDisplayNeedsUpdate;
	static const JUtf8Byte* kDisplayChanged;

	class DisplayNeedsUpdate : public JBroadcaster::Message
		{
		public:

			DisplayNeedsUpdate()
				:
				JBroadcaster::Message(kDisplayNeedsUpdate)
				{ };
		};

	class DisplayChanged : public JBroadcaster::Message
		{
		public:

			DisplayChanged(const JIndex index)
				:
				JBroadcaster::Message(kDisplayChanged),
				itsIndex(index)
				{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

		private:

			JIndex itsIndex;
		};
};


/******************************************************************************
 GetSelectedDisplayIndex

 ******************************************************************************/

inline JIndex
JXDisplayMenu::GetSelectedDisplayIndex()
	const
{
	return itsDisplayIndex;
}

#endif
