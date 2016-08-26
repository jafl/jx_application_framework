/******************************************************************************
 JXDisplayMenu.h

	Interface for the JXDisplayMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDisplayMenu
#define _H_JXDisplayMenu

#include <JXTextMenu.h>

class JXOpenDisplayDialog;

class JXDisplayMenu : public JXTextMenu
{
public:

	JXDisplayMenu(const JCharacter* title, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	JXDisplayMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXDisplayMenu();

	void		SelectCurrentDisplay() const;
	JXDisplay*	GetSelectedDisplay() const;
	JIndex		GetSelectedDisplayIndex() const;

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex	itsDisplayIndex;
	JIndex	itsNewDisplayIndex;

	// used when opening a new display

	JXOpenDisplayDialog*	itsNewDisplayDialog;

private:

	void	BuildMenu();
	void	ChooseDisplay(const JIndex index);

	// not allowed

	JXDisplayMenu(const JXDisplayMenu& source);
	const JXDisplayMenu& operator=(const JXDisplayMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kDisplayNeedsUpdate;
	static const JCharacter* kDisplayChanged;

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
