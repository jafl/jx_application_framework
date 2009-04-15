/******************************************************************************
 JXTabGroup.h

	Copyright © 2002-08 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTabGroup
#define _H_JXTabGroup

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCardFile.h>
#include <JPtrArray-JString.h>
#include <JFontStyle.h>

class JXTextMenu;
class JXImage;

class JXTabGroup : public JXWidget
{
public:

	enum Edge
	{
		kTop,
		kLeft,
		kBottom,
		kRight
	};

public:

	JXTabGroup(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXTabGroup();

	virtual void	Activate();		// must call inherited

	JSize		GetTabCount() const;
	JBoolean	GetCurrentTabIndex(JIndex* index) const;

	JXWidgetSet*	InsertTab(const JIndex index, const JCharacter* title,
							  const JBoolean closeable = kJFalse);
	JXWidgetSet*	PrependTab(const JCharacter* title, const JBoolean closeable = kJFalse);
	JXWidgetSet*	AppendTab(const JCharacter* title, const JBoolean closeable = kJFalse);

	void			InsertTab(const JIndex index, const JCharacter* title,
							  JXWidgetSet* card, const JBoolean closeable = kJFalse);
	void			PrependTab(const JCharacter* title, JXWidgetSet* card,
							   const JBoolean closeable = kJFalse);
	void			AppendTab(const JCharacter* title, JXWidgetSet* card,
							  const JBoolean closeable = kJFalse);

	JXWidgetSet*	RemoveTab(const JIndex index);
	void			DeleteTab(const JIndex index);
	void			KillFocusOnCurrentTab();

	const JString&	GetTabTitle(const JIndex index) const;
	void			SetTabTitle(const JIndex index, const JCharacter* title);

	JBoolean	TabCanClose(const JIndex index) const;
	void		SetTabCanClose(const JIndex index, const JBoolean closable);

	JBoolean		ShowTab(const JIndex index);
	JBoolean		ShowTab(JXWidgetSet* card);
	void			ShowPreviousTab();
	void			ShowNextTab();
	void			ScrollTabsIntoView();

	Edge	GetTabEdge() const;
	void	SetTabEdge(const Edge edge);

	const JString&		GetFontName() const;
	void				SetFontName(const JCharacter* name);

	JSize				GetFontSize() const;
	void				SetFontSize(const JSize size);

	const JFontStyle&	GetFontStyle() const;
	void				SetFontStyle(const JFontStyle& style);

	void				SetFont(const JCharacter* name, const JSize size,
								const JFontStyle& style);

	JCoordinate	GetTabTitlePreMargin(const JIndex index) const;
	void		SetTabTitlePreMargin(const JIndex index, const JCoordinate margin);
	JCoordinate	GetTabTitlePostMargin(const JIndex index) const;
	void		SetTabTitlePostMargin(const JIndex index, const JCoordinate margin);

	JXContainer*	GetCardEnclosure();

	void		ReadSetup(istream& input);
	static void	SkipSetup(istream& input);
	void		WriteSetup(ostream& output) const;

protected:

	JBoolean			FindTab(const JPoint& pt, JIndex* index, JRect* rect) const;
	JBoolean			GetMouseTabIndex(JIndex* index) const;
	virtual JBoolean	OKToDeleteTab(const JIndex index);

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void	DrawTab(const JIndex index, JXWindowPainter& p,
							const JRect& rect, const Edge edge);

	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);

	virtual void	HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseLeave();

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	struct TabInfo
	{
		JBoolean	closable;
		JCoordinate	preMargin;
		JCoordinate	postMargin;

		TabInfo();
		TabInfo(const JBoolean closable);

		TabInfo(const JBoolean c, const JCoordinate pre, const JCoordinate post)
			:
			closable(c), preMargin(pre), postMargin(post)
		{ };
	};

	enum DragAction
	{
		kInvalidClick,
		kScrollUp,
		kScrollDown,
		kClose
	};

private:

	Edge				itsEdge;
	JString				itsFontName;
	JSize				itsFontSize;
	JFontStyle			itsFontStyle;
	JPtrArray<JString>*	itsTitles;
	JArray<TabInfo>*	itsTabInfoList;
	JBoolean			itsCanScrollUpFlag;
	JRect				itsScrollUpRect;
	JBoolean			itsCanScrollDownFlag;
	JRect				itsScrollDownRect;
	JIndex				itsFirstDrawIndex;
	JIndex				itsLastDrawIndex;
	JArray<JRect>*		itsTabRects;		// 1 <=> itsFirstDrawIndex
	JXCardFile*			itsCardFile;
	JXTextMenu*			itsContextMenu;		// NULL until first used
	JXImage*			itsCloseImage;
	JXImage*			itsClosePushedImage;
	JIndex				itsPrevTabIndex;	// 0 => none

	// used during dragging

	DragAction	itsDragAction;
	JBoolean	itsScrollUpPushedFlag;
	JBoolean	itsScrollDownPushedFlag;

	JIndex		itsMouseIndex;
	JRect		itsCloseRect;
	JBoolean	itsClosePushedFlag;

private:

	void	UpdateAppearance();
	void	PlaceCardFile();
	void	DrawTabBorder(JXWindowPainter& p, const JRect& rect,
						  const JBoolean isSelected);
	void	DrawCloseButton(const JIndex index, JXWindowPainter& p,
							const JRect& rect);
	void	DrawScrollButtons(JXWindowPainter& p, const JCoordinate lineHeight);
	void	ScrollWait(const JFloat delta) const;

	void	CreateContextMenu();
	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	ScrollUpToTab(const JIndex index);

	// not allowed

	JXTabGroup(const JXTabGroup& source);
	const JXTabGroup& operator=(const JXTabGroup& source);

public:

	// JBroadcaster messages

	static const JCharacter* kAppearanceChanged;

	class AppearanceChanged : public JBroadcaster::Message
		{
		public:

			AppearanceChanged()
				:
				JBroadcaster::Message(kAppearanceChanged)
				{ };
		};
};


/******************************************************************************
 GetTabCount

 ******************************************************************************/

inline JSize
JXTabGroup::GetTabCount()
	const
{
	return itsTitles->GetElementCount();
}

/******************************************************************************
 GetCurrentTabIndex

 ******************************************************************************/

inline JBoolean
JXTabGroup::GetCurrentTabIndex
	(
	JIndex* index
	)
	const
{
	return itsCardFile->GetCurrentCardIndex(index);
}

/******************************************************************************
 GetMouseTabIndex (protected)

 ******************************************************************************/

inline JBoolean
JXTabGroup::GetMouseTabIndex
	(
	JIndex* index
	)
	const
{
	*index = itsMouseIndex;
	return JI2B( itsMouseIndex > 0 );
}

/******************************************************************************
 PrependTab

 ******************************************************************************/

inline JXWidgetSet*
JXTabGroup::PrependTab
	(
	const JCharacter*	title,
	const JBoolean		closeable
	)
{
	return InsertTab(1, title, closeable);
}

inline void
JXTabGroup::PrependTab
	(
	const JCharacter*	title,
	JXWidgetSet*		card,
	const JBoolean		closeable
	)
{
	InsertTab(1, title, card, closeable);
}

/******************************************************************************
 AppendTab

 ******************************************************************************/

inline JXWidgetSet*
JXTabGroup::AppendTab
	(
	const JCharacter*	title,
	const JBoolean		closeable
	)
{
	return InsertTab(GetTabCount()+1, title, closeable);
}

inline void
JXTabGroup::AppendTab
	(
	const JCharacter*	title,
	JXWidgetSet*		card,
	const JBoolean		closeable
	)
{
	InsertTab(GetTabCount()+1, title, card, closeable);
}

/******************************************************************************
 DeleteTab

	Deletes the specified tab and everything on it.

 ******************************************************************************/

inline void
JXTabGroup::DeleteTab
	(
	const JIndex index
	)
{
	JXWidgetSet* card = RemoveTab(index);
	delete card;
}

/******************************************************************************
 KillFocusOnCurrentTab

	If one of the widgets on the current card has focus, we call KillFocus().
	This is useful when you want to discard the current card without saving
	its contents.

 ******************************************************************************/

inline void
JXTabGroup::KillFocusOnCurrentTab()
{
	itsCardFile->KillFocusOnCurrentCard();
}

/******************************************************************************
 Tab edge

 ******************************************************************************/

inline JXTabGroup::Edge
JXTabGroup::GetTabEdge()
	const
{
	return itsEdge;
}

inline void
JXTabGroup::SetTabEdge
	(
	const Edge edge
	)
{
	itsEdge = edge;
	UpdateAppearance();
}

/******************************************************************************
 Tab title

 ******************************************************************************/

inline const JString&
JXTabGroup::GetTabTitle
	(
	const JIndex index
	)
	const
{
	return *(itsTitles->NthElement(index));
}

inline void
JXTabGroup::SetTabTitle
	(
	const JIndex		index,
	const JCharacter*	title
	)
{
	*(itsTitles->NthElement(index)) = title;
	Refresh();
}

/******************************************************************************
 Tab closable

 ******************************************************************************/

inline JBoolean
JXTabGroup::TabCanClose
	(
	const JIndex index
	)
	const
{
	return (itsTabInfoList->GetElement(index)).closable;
}

inline void
JXTabGroup::SetTabCanClose
	(
	const JIndex	index,
	const JBoolean	closable
	)
{
	TabInfo info = itsTabInfoList->GetElement(index);
	if (closable != info.closable)
		{
		info.closable = closable;
		itsTabInfoList->SetElement(index, info);
		Refresh();
		}
}

/******************************************************************************
 Get font info

 ******************************************************************************/

inline const JString&
JXTabGroup::GetFontName()
	const
{
	return itsFontName;
}

inline JSize
JXTabGroup::GetFontSize()
	const
{
	return itsFontSize;
}

inline const JFontStyle&
JXTabGroup::GetFontStyle()
	const
{
	return itsFontStyle;
}

/******************************************************************************
 GetCardEnclosure

	Returns the enclosure that must be used for all cards.

 ******************************************************************************/

inline JXContainer*
JXTabGroup::GetCardEnclosure()
{
	return itsCardFile;
}

/******************************************************************************
 UpdateAppearance (private)

 ******************************************************************************/

inline void
JXTabGroup::UpdateAppearance()
{
	itsFirstDrawIndex = 1;
	PlaceCardFile();
	Refresh();
	Broadcast(AppearanceChanged());
}

#endif
