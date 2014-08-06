/******************************************************************************
 SCNodeMenu.h

	Interface for the SCNodeMenu class

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCNodeMenu
#define _H_SCNodeMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class SCCircuit;

class SCNodeMenu : public JXTextMenu
{
public:

	SCNodeMenu(const SCCircuit* circuit, const JCharacter* title,
			   JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	SCNodeMenu(const SCCircuit* circuit,
			   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~SCNodeMenu();

	JIndex	GetNodeIndex() const;
	void	SetNodeIndex(const JIndex node);

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const SCCircuit*	itsCircuit;
	JIndex				itsNodeIndex;
	JBoolean			itsBroadcastNodeChangeFlag;

private:

	void	SCNodeMenuX(const SCCircuit* circuit);
	void	BuildMenu();

	// not allowed

	SCNodeMenu(const SCNodeMenu& source);
	const SCNodeMenu& operator=(const SCNodeMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kNodeNeedsUpdate;
	static const JCharacter* kNodeChanged;

	class NodeNeedsUpdate : public JBroadcaster::Message
		{
		public:

			NodeNeedsUpdate()
				:
				JBroadcaster::Message(kNodeNeedsUpdate)
				{ };
		};

	class NodeChanged : public JBroadcaster::Message
		{
		public:

			NodeChanged(const JIndex node)
				:
				JBroadcaster::Message(kNodeChanged),
				itsNodeIndex(node)
				{ };

			JIndex
			GetNodeIndex() const
			{
				return itsNodeIndex;
			};

		private:

			JIndex itsNodeIndex;
		};
};

/******************************************************************************
 GetNodeIndex

 ******************************************************************************/

inline JIndex
SCNodeMenu::GetNodeIndex()
	const
{
	return itsNodeIndex;
}

#endif
