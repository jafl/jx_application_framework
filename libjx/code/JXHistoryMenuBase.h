/******************************************************************************
 JXHistoryMenuBase.h

	Interface for the JXHistoryMenuBase class

	Copyright (C) 1998-05 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXHistoryMenuBase
#define _H_JXHistoryMenuBase

#include "JXTextMenu.h"

class JXInputField;

class JXHistoryMenuBase : public JXTextMenu
{
public:

	enum HistoryDirection
	{
		kNewestItemAtTop,
		kNewestItemAtBottom
	};

public:

	~JXHistoryMenuBase() override;

	JSize	GetHistoryLength() const;
	void	SetHistoryLength(const JSize historyLength);
	void	ClearHistory();

	HistoryDirection	GetHistoryDirection() const;
	void				SetHistoryDirection(const HistoryDirection direction);

	void	SetDefaultIcon(JXImage* icon, const bool menuOwnsIcon);

	JIndex	GetFirstIndex() const;
	void	SetFirstIndex(const JIndex index);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	const JString&	GetItemText(const Message& message) const;
	void			UpdateInputField(const Message& message, JXInputField* input) const;

	static void	ReadSetup(std::istream& input, JPtrArray<JString>* itemList,
						  JPtrArray<JString>* nmShortcutList);

protected:

	JXHistoryMenuBase(const JSize historyLength,
					  const JString& title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXHistoryMenuBase(const JSize historyLength,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	void	AddItem(const JString& text, const JString& nmShortcut);
	void	AdjustLength();

	virtual void	UpdateMenu();		// must call inherited
	virtual void	UpdateItemImage(const JIndex index);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex				itsFirstIndex;
	JSize				itsHistoryLength;
	HistoryDirection	itsHistoryDirection;
	JXImage*			itsDefaultIcon;
	bool				itsOwnsDefIconFlag;

private:

	void	JXHistoryMenuBaseX(const JSize historyLength);

	static void	ReadSetup(std::istream& input, JXHistoryMenuBase* menu,
						  JPtrArray<JString>* itemList,
						  JPtrArray<JString>* nmShortcutList);
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
 History direction

 ******************************************************************************/

inline JXHistoryMenuBase::HistoryDirection
JXHistoryMenuBase::GetHistoryDirection()
	const
{
	return itsHistoryDirection;
}

inline void
JXHistoryMenuBase::SetHistoryDirection
	(
	const HistoryDirection direction
	)
{
	itsHistoryDirection = direction;
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
