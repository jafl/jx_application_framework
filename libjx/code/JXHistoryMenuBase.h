/******************************************************************************
 JXHistoryMenuBase.h

	Interface for the JXHistoryMenuBase class

	Copyright © 1998-05 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHistoryMenuBase
#define _H_JXHistoryMenuBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXInputField;

class JXHistoryMenuBase : public JXTextMenu
{
public:

	virtual ~JXHistoryMenuBase();

	JSize	GetHistoryLength() const;
	void	SetHistoryLength(const JSize historyLength);
	void	ClearHistory();

	void	SetDefaultIcon(JXImage* icon, const JBoolean menuOwnsIcon);

	JIndex	GetFirstIndex() const;
	void	SetFirstIndex(const JIndex index);

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;

	const JString&	GetItemText(const Message& message) const;
	void			UpdateInputField(const Message& message, JXInputField* input) const;

	static void	ReadSetup(istream& input, JPtrArray<JString>* itemList,
						  JPtrArray<JString>* nmShortcutList);

protected:

	JXHistoryMenuBase(const JSize historyLength,
					  const JCharacter* title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXHistoryMenuBase(const JSize historyLength,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	void	AddItem(const JCharacter* text, const JCharacter* nmShortcut);
	void	AdjustLength();

	virtual void	UpdateMenu();		// must call inherited
	virtual void	UpdateItemImage(const JIndex index);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex		itsFirstIndex;
	JSize		itsHistoryLength;
	JXImage*	itsDefaultIcon;
	JBoolean	itsOwnsDefIconFlag;

private:

	void	JXHistoryMenuBaseX(const JSize historyLength);

	static void	ReadSetup(istream& input, JXHistoryMenuBase* menu,
						  JPtrArray<JString>* itemList,
						  JPtrArray<JString>* nmShortcutList);

	// not allowed

	JXHistoryMenuBase(const JXHistoryMenuBase& source);
	const JXHistoryMenuBase& operator=(const JXHistoryMenuBase& source);
};


/******************************************************************************
 GetHistoryLength

 ******************************************************************************/

inline JSize
JXHistoryMenuBase::GetHistoryLength()
	const
{
	return itsHistoryLength;
}

/******************************************************************************
 ClearHistory

 ******************************************************************************/

inline void
JXHistoryMenuBase::ClearHistory()
{
	while (GetItemCount() >= itsFirstIndex)
		{
		RemoveItem(GetItemCount());
		}
}

/******************************************************************************
 Index of first history item

 ******************************************************************************/

inline JIndex
JXHistoryMenuBase::GetFirstIndex()
	const
{
	return itsFirstIndex;
}

inline void
JXHistoryMenuBase::SetFirstIndex
	(
	const JIndex index
	)
{
	itsFirstIndex = index;
}

#endif
