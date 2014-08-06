/******************************************************************************
 SCComponentMenu.h

	Interface for the SCComponentMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCComponentMenu
#define _H_SCComponentMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class SCCircuit;
class SCComponent;

typedef JBoolean (SCComponentFilter)(const SCComponent& comp);

class SCComponentMenu : public JXTextMenu
{
public:

	SCComponentMenu(const SCCircuit* circuit, SCComponentFilter* filter,
					const JCharacter* title, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	SCComponentMenu(const SCCircuit* circuit, SCComponentFilter* filter,
					JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~SCComponentMenu();

	JBoolean	GetCompIndex(JIndex* compIndex) const;
	JBoolean	SetCompIndex(const JIndex compIndex);
	JBoolean	CompIndexValid(const JIndex compIndex) const;

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const SCCircuit*	itsCircuit;
	SCComponentFilter*	itsFilter;
	JArray<JIndex>*		itsCompList;
	JIndex				itsMenuIndex;
	JBoolean			itsBroadcastCompChangeFlag;

private:

	void	SCComponentMenuX(const SCCircuit* circuit, SCComponentFilter* filter);
	void	BuildMenu();

	void		BuildCompList();
	JBoolean	MenuIndexToCompIndex(const JIndex menuIndex, JIndex* compIndex) const;
	JBoolean	CompIndexToMenuIndex(const JIndex compIndex, JIndex* menuIndex) const;

	// not allowed

	SCComponentMenu(const SCComponentMenu& source);
	const SCComponentMenu& operator=(const SCComponentMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kComponentNeedsUpdate;
	static const JCharacter* kComponentChanged;

	class ComponentNeedsUpdate : public JBroadcaster::Message
		{
		public:

			ComponentNeedsUpdate()
				:
				JBroadcaster::Message(kComponentNeedsUpdate)
				{ };
		};

	class ComponentChanged : public JBroadcaster::Message
		{
		public:

			ComponentChanged(const JIndex compIndex)
				:
				JBroadcaster::Message(kComponentChanged),
				itsCompIndex(compIndex)
				{ };

			JIndex
			GetComponentIndex() const
			{
				return itsCompIndex;
			};

		private:

			JIndex itsCompIndex;
		};
};

JBoolean SCACSourceFilter(const SCComponent& comp);
JBoolean SCDepSourceFilter(const SCComponent& comp);
JBoolean SCPassiveLinearFilter(const SCComponent& comp);


/******************************************************************************
 GetCompIndex

	Returns kJTrue if the current selection is valid.  One possibility for
	kJFalse is that there are no valid components in the circuit.

 ******************************************************************************/

inline JBoolean
SCComponentMenu::GetCompIndex
	(
	JIndex* compIndex
	)
	const
{
	return MenuIndexToCompIndex(itsMenuIndex, compIndex);
}

#endif
